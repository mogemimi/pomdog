// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Utility/Assert.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
// NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../Network.Win32/IOServiceWin32.hpp"
#else
#error "Platform undefined or not supported."
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

IOService::IOService(const GameClock* clockIn)
    : clock(clockIn)
{
}

std::unique_ptr<Error> IOService::Initialize()
{
#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = Detail::Win32::PrepareNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

std::unique_ptr<Error> IOService::Shutdown()
{
#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = Detail::Win32::ShutdownNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

void IOService::Step()
{
    tasks();
}

Connection IOService::ScheduleTask(std::function<void()>&& func)
{
    return tasks.Connect(std::move(func));
}

TimePoint IOService::GetNowTime() const
{
    POMDOG_ASSERT(clock != nullptr);
    return TimePoint{clock->GetTotalGameTime()};
}

} // namespace Pomdog
