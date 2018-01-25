// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../Platform.Apple/TimeSourceApple.hpp"
#elif defined(POMDOG_PLATFORM_WIN32)
#include "../Platform.Win32/TimeSourceWin32.hpp"
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
#include "../Platform.Linux/TimeSourceLinux.hpp"
#else
#error "Platform undefined or not supported."
#endif

namespace Pomdog {
namespace Detail {

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
using TimeSource = Detail::Apple::TimeSourceApple;
#elif defined(POMDOG_PLATFORM_WIN32)
using TimeSource = Detail::Win32::TimeSourceWin32;
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
using TimeSource = Detail::Linux::TimeSourceLinux;
#else
#error "Platform undefined or not supported."
#endif

} // namespace Detail
} // namespace Pomdog
