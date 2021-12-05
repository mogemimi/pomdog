// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS) && defined(POMDOG_USING_LIBRARY_EXPORTS)
#error "exporting error"
#endif

#if defined(_MSC_VER) && defined(_WIN32)
#   // MSVC
#   if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __declspec(dllexport)
#   elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __declspec(dllimport)
#   else
#       define POMDOG_EXPORT
#   endif
#elif defined(__CYGWIN__) && defined(__GNUC__)
#   // Cygwin
#   if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __attribute__((dllexport))
#   elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __attribute__((dllimport))
#   else
#       define POMDOG_EXPORT
#   endif
#elif defined(__clang__) && defined(__APPLE_CC__)
#   // Clang for Apple OS X
#   if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __attribute__((visibility("default")))
#   elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT
#   else
#       define POMDOG_EXPORT
#   endif
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#   // GCC for Linux
#   if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT __attribute__((visibility("default")))
#   elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#       define POMDOG_EXPORT
#   else
#       define POMDOG_EXPORT
#   endif
#endif
