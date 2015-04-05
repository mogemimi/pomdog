// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMESOURCE_C4211975_HPP
#define POMDOG_TIMESOURCE_C4211975_HPP

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX)
#include "../Platform.Cocoa/TimeSourceCocoa.hpp"
#elif defined(POMDOG_PLATFORM_WIN32)
#include "../Platform.Win32/TimeSourceWin32.hpp"
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
#include "../Platform.Linux/TimeSourceLinux.hpp"
#else
#error "Platform undefined or not supported."
#endif

namespace Pomdog {
namespace Detail {

#if defined(POMDOG_PLATFORM_MACOSX)
using TimeSource = Detail::Cocoa::TimeSourceCocoa;
#elif defined(POMDOG_PLATFORM_WIN32)
using TimeSource = Detail::Win32::TimeSourceWin32;
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
using TimeSource = Detail::Linux::TimeSourceLinux;
#else
#error "Platform undefined or not supported."
#endif

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_TIMESOURCE_C4211975_HPP
