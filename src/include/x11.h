#ifndef X11SINGLETON_H
#define X11SINGLETON_H

#include "errorhandler.h"

// For Linux (X11)
#ifdef __linux__
#include <X11/XKBlib.h>
#endif

// For macOS
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hidsystem/ev_keymap.h>
#endif

// For Windows
#ifdef _WIN32
#include <windows.h>
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

#ifdef __linux__
    Display *d = nullptr;
#endif

    X11Singleton(X11Singleton const&) = delete;
    void operator=(X11Singleton const&) = delete;
};

#endif // X11SINGLETON_H
