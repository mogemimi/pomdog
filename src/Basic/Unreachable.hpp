// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#if defined(__clang__)
#if __has_builtin(__builtin_unreachable)
#define POMDOG_UNREACHABLE(message) __builtin_unreachable()
#else
#define POMDOG_UNREACHABLE(message)
#endif
#elif defined(__GNUC__)
#define POMDOG_UNREACHABLE(message) __builtin_unreachable()
#elif defined(_MSC_VER)
#define POMDOG_UNREACHABLE(message) __assume(0)
#else
#define POMDOG_UNREACHABLE(message)
#endif
