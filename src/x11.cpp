#include "x11.h"

// Use the standard namespace
using namespace std;

#if (MAC_OS_X_VERSION_MAX_ALLOWED < 120000) // Before macOS 12 Monterey
  #define kIOMainPortDefault kIOMasterPortDefault
#endif

#ifdef __linux__
// Define constants for better readability
const unsigned CAPS_MASK = 0x01;
#endif

X11Singleton::X11Singleton() {
#ifdef __linux__
    d = nullptr;
    openDisplay();
#endif
}

X11Singleton::~X11Singleton() {
#ifdef __linux__
    closeDisplay();
#endif
}

void X11Singleton::openDisplay() {
#ifdef __linux__
    if (d == nullptr) {
        const char* displayEnv = getenv("DISPLAY");
        d = XOpenDisplay(displayEnv);
        if (d == nullptr) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::X11_DISPLAY_ERROR);
        }
    }
#endif
}

void X11Singleton::closeDisplay() {
#ifdef __linux__
    if (d) {
        XCloseDisplay(d);
        d = nullptr;
    }
#endif
}

bool X11Singleton::isCapsLockOn() {
#ifdef __linux__
    if (d == nullptr) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::X11_DISPLAY_ERROR);
    }

    unsigned n;
    if (XkbGetIndicatorState(d, XkbUseCoreKbd, &n) != Success) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::X11_DISPLAY_ERROR);
    }
    return (n & CAPS_MASK) == 1;

#elif defined(__APPLE__)
    kern_return_t kr;
    io_service_t ios;
    io_connect_t ioc;
    CFMutableDictionaryRef mdict;

    mdict = IOServiceMatching(kIOHIDSystemClass);
    ios = IOServiceGetMatchingService(kIOMainPortDefault, (CFDictionaryRef) mdict);
    if (!ios) {
        if (mdict) {
            CFRelease(mdict);
        }
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::X11_SYSTEM_ERROR);
        return false;
    }

    kr = IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
    IOObjectRelease(ios);
    if (kr != KERN_SUCCESS) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::X11_SYSTEM_ERROR);
        return false;
    }

    bool state;
    kr = IOHIDGetModifierLockState(ioc, kIOHIDCapsLockState, &state);
    if (kr != KERN_SUCCESS) {
        IOServiceClose(ioc);
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::X11_SYSTEM_ERROR);
        return false;
    }
    IOServiceClose(ioc);
    return state;

#elif defined(_WIN32)
    // Check for Caps Lock on Windows
    SHORT state = GetKeyState(VK_CAPITAL);
    bool isOn = (state != 0) && ((state & 0x0001) != 0);  // Adjusted condition
    if (state == 0) {
        isOn = false;  // Explicitly set to false if state is 0
    }
    return isOn;

#else
    ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::UNSUPPORTED_PLATFORM_ERROR);
    return false;

#endif
}
