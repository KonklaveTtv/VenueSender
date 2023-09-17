#ifndef X11SINGLETON_H
#define X11SINGLETON_H

#include "errorhandler.h"
#include "fileutils.h"

// Platform-specific includes
#if defined(__linux__)
    #include <X11/XKBlib.h>
#elif defined(__APPLE__)
    #include <CoreFoundation/CoreFoundation.h>
    #include <IOKit/IOKitLib.h>
    #include <IOKit/hidsystem/IOHIDLib.h>
    #include <IOKit/hidsystem/IOHIDParameter.h>
    #include <IOKit/hidsystem/ev_keymap.h>
#elif defined(_WIN32)
    #include <windows.h>
#else
    #error "Unsupported platform"
#endif

#include <iostream>

#if (MAC_OS_X_VERSION_MAX_ALLOWED < 120000) // Before macOS 12 Monterey
  #define kIOMainPortDefault kIOMasterPortDefault
#endif

#ifdef __linux__
const unsigned CAPS_MASK = 0x01;
#endif

#if defined (_WIN32)
    unsigned CAPS_LOCK = 0x0001;
#endif

// Define a macro to check for macOS Monterey or newer
#define MACOS_MONTEREY_OR_NEWER (MAC_OS_X_VERSION_MAX_ALLOWED >= 120000)

// Declare the macOS Caps Lock checking function for Monterey and newer
#if MACOS_MONTEREY_OR_NEWER
bool isMacOSCapsLockOn();
#endif

class X11Singleton {
public:
    static X11Singleton& getInstance() {
        static X11Singleton instance;
        return instance;
    }

    bool isCapsLockOn();
    void openDisplay();  // New method
    void closeDisplay(); // New method

private:
    X11Singleton();  // Constructor is private
    ~X11Singleton(); // So is the destructor

    // Platform-specific members
#if defined(__linux__)
    Display *d = nullptr;
#endif

    X11Singleton(X11Singleton const&) = delete;
    void operator=(X11Singleton const&) = delete;
};

#endif // X11SINGLETON_H
