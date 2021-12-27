// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "pomdog/chrono/apple/time_source_apple.h"
#elif defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/chrono/win32/time_source_win32.h"
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
#include "pomdog/chrono/linux/time_source_linux.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/chrono/emscripten/time_source_emscripten.h"
#else
#error "Platform undefined or not supported."
#endif

namespace pomdog::detail {

[[nodiscard]] std::shared_ptr<TimeSource>
makeTimeSource() noexcept
{
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
    auto timeSource = std::make_shared<pomdog::detail::apple::TimeSourceApple>();
#elif defined(POMDOG_PLATFORM_WIN32)
    auto timeSource = std::make_shared<pomdog::detail::win32::TimeSourceWin32>();
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
    auto timeSource = std::make_shared<pomdog::detail::linux::TimeSourceLinux>();
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
    auto timeSource = std::make_shared<pomdog::detail::emscripten::TimeSourceEmscripten>();
#else
#error "Platform undefined or not supported."
#endif

#if !defined(__clang__) && defined(__GNUC__)
    return timeSource;
#else
    return std::move(timeSource);
#endif
}

} // namespace pomdog::detail
