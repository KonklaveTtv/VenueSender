#include "x11.h"

// Use the standard namespace
using namespace std;

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
    CFMutableDictionaryRef mdict;
    mdict = IOServiceMatching(kIOHIDSystemClass);
    io_service_t ios;
    ios = IOServiceGetMatchingService(kIOMainPortDefault, (CFDictionaryRef) mdict);
    if (!ios) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::X11_SYSTEM_ERROR);
    }

    bool state = false;
    CFTypeRef cfState;

    cfState = IORegistryEntryCreateCFProperty(ios, CFSTR(kIOHIDCapsLockStateKey), kCFAllocatorDefault, 0);
    if (cfState) {
        state = CFBooleanGetValue((CFBooleanRef)cfState);
        CFRelease(cfState);
    }

    IOObjectRelease(ios);
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
