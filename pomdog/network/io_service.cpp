// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/io_service.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/platform.hpp"
#include "pomdog/utility/assert.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) ||     \
    defined(POMDOG_PLATFORM_APPLE_IOS) ||  \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
// NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/io_service_win32.hpp"
#else
#error "Platform undefined or not supported."
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

IOService::IOService() noexcept = default;

std::unique_ptr<Error>
IOService::Initialize(const std::shared_ptr<GameClock>& clock)
{
    POMDOG_ASSERT(clock != nullptr);
    clock_ = clock;

#if defined(POMDOG_PLATFORM_MACOSX) ||     \
    defined(POMDOG_PLATFORM_APPLE_IOS) ||  \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = detail::win32::PrepareNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

std::unique_ptr<Error> IOService::Shutdown()
{
#if defined(POMDOG_PLATFORM_MACOSX) ||     \
    defined(POMDOG_PLATFORM_APPLE_IOS) ||  \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = detail::win32::ShutdownNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

void IOService::Step()
{
    tasks_();
}

Connection IOService::ScheduleTask(std::function<void()>&& func)
{
    return tasks_.Connect(std::move(func));
}

TimePoint IOService::GetNowTime() const
{
    POMDOG_ASSERT(clock_ != nullptr);
    return TimePoint{clock_->GetTotalGameTime()};
}

} // namespace pomdog
