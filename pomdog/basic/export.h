// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS) && defined(POMDOG_USING_LIBRARY_EXPORTS)
#error "exporting error"
#endif

#if (defined(_MSC_VER) && defined(_WIN32)) || (defined(__clang__) && defined(_MSC_VER))
// MSVC or clang-cl for Windows
#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT __declspec(dllexport)
#elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT __declspec(dllimport)
#else
#define POMDOG_EXPORT
#endif

#elif defined(__clang__) && defined(__APPLE_CC__)
// Clang for Apple macOS
#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT __attribute__((visibility("default")))
#elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT
#else
#define POMDOG_EXPORT
#endif

#elif defined(__clang__)
// Clang on Linux / other Unix-like platforms
#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT __attribute__((visibility("default")))
#elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT
#else
#define POMDOG_EXPORT
#endif

#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
// GCC for Linux
#if defined(POMDOG_BUILDING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT __attribute__((visibility("default")))
#elif defined(POMDOG_USING_LIBRARY_EXPORTS)
#define POMDOG_EXPORT
#else
#define POMDOG_EXPORT
#endif

#else
// NOTE: Unknown compiler; define POMDOG_EXPORT as empty to avoid compile errors.
#define POMDOG_EXPORT
#endif
