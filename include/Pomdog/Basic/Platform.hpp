// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#ifdef __APPLE_CC__
#include <TargetConditionals.h>
#endif

// NOTE: Platform detection
#if defined(linux) || defined(__linux) || defined(__linux__)
    // Linux
    #define POMDOG_PLATFORM_LINUX
#elif defined(__ORBIS__)
    // PlayStation 4
    #define POMDOG_PLATFORM_PLAYSTATION4
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    // BSD
    #define POMDOG_PLATFORM_BSD
#elif defined(_XBOX_ONE)
    // Xbox One
    #define POMDOG_PLATFORM_XBOX_ONE
#elif defined(__NX__)
    // Switch
    #define POMDOG_PLATFORM_SWITCH
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    // Windows
    #define POMDOG_PLATFORM_WIN32
#elif defined(__EMSCRIPTEN__)
    // Emscripten
    #define POMDOG_PLATFORM_EMSCRIPTEN
#elif defined(ANDROID) || defined(__ANDROID__)
    // Android OS
    #define POMDOG_PLATFORM_ANDROID
#elif defined(__APPLE_CC__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    // Apple iOS
    #define POMDOG_PLATFORM_APPLE_IOS
#elif defined(__APPLE_CC__) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
    // Mac OS X
    #define POMDOG_PLATFORM_MACOSX
#elif defined(__QNXNTO__)
    // QNX Neutrino
    #define POMDOG_PLATFORM_QNXNTO
#elif defined(__native_client__)
    // Google Native Client (NaCl)
    #define POMDOG_PLATFORM_NACL
#elif defined(__CYGWIN__)
    // Cygwin
    #define POMDOG_PLATFORM_CYGWIN
#elif defined(sun) || defined(__sun)
    // Solaris
    #define POMDOG_PLATFORM_SOLARIS
#elif defined(__hpux)
    // Hewlett-Packard UNIX
    #define POMDOG_PLATFORM_HPUX
#elif defined(__ghs__)
    // Wii U
    #define POMDOG_PLATFORM_WIIU
#else
    #error "Sorry, this platform is not supported."
#endif
