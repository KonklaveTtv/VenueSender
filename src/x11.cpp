#include "x11.h"

X11Singleton::X11Singleton() {
#ifdef __linux__
    const char* displayEnv = getenv("DISPLAY");
    d = XOpenDisplay(displayEnv);
    if (d == nullptr) {
        std::cerr << "Failed to open X11 display.\n";
    }
#endif
}

X11Singleton::~X11Singleton() {
#ifdef __linux__
    if (d) {
        XCloseDisplay(d);
    }
#endif
}

bool X11Singleton::isCapsLockOn() {
#ifdef __linux__
    if (d == nullptr) {
        return false;
    }

    unsigned n;
    if (XkbGetIndicatorState(d, XkbUseCoreKbd, &n) != Success) {
        std::cerr << "Failed to get the indicator state from X11.\n";
        return false;
    }
    return (n & 0x01) == 1;

#elif defined(__APPLE__)
    io_service_t keyService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleEmbeddedKeyboard"));
    if (keyService == IO_OBJECT_NULL) {
        std::cerr << "Failed to get IOService for keyboard.\n";
        return false;
    }

    CFTypeRef state = IORegistryEntryCreateCFProperty(keyService, CFSTR(kIOHIDKeyboardCapsLockState), kCFAllocatorDefault, 0);
    if (state == nullptr) {
        std::cerr << "Failed to get Caps Lock state.\n";
        IOObjectRelease(keyService);
        return false;
    }

    bool isOn = CFBooleanGetValue((CFBooleanRef)state);
    CFRelease(state);
    IOObjectRelease(keyService);
    return isOn;

#elif defined(_WIN32)
    SHORT state = GetKeyState(VK_CAPITAL);
    if (state == 0) {
        std::cerr << "Failed to get Caps Lock state.\n";
        return false;
    }

    return (state & 0x0001) != 0;

#else
    std::cerr << "Caps Lock check not supported on this platform.\n";
    return false;

#endif
}
