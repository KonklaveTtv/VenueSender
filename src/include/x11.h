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
