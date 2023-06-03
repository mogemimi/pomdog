// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/io_service.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/assert.h"

#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
// NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/io_service_win32.h"
#else
#error "Platform undefined or not supported."
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

IOService::IOService() noexcept = default;

std::unique_ptr<Error>
IOService::initialize(const std::shared_ptr<GameClock>& clock)
{
    POMDOG_ASSERT(clock != nullptr);
    clock_ = clock;

#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = detail::win32::prepareNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

std::unique_ptr<Error> IOService::shutdown()
{
#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN) || \
    defined(POMDOG_PLATFORM_LINUX)
    // NOTE: nothing to do
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto err = detail::win32::shutdownNetworkService(); err != nullptr) {
        return err;
    }
#else
#error "Platform undefined or not supported."
#endif
    return nullptr;
}

void IOService::step()
{
    tasks_();
}

Connection IOService::scheduleTask(std::function<void()>&& func)
{
    return tasks_.connect(std::move(func));
}

TimePoint IOService::getNowTime() const
{
    POMDOG_ASSERT(clock_ != nullptr);
    return TimePoint{clock_->getTotalGameTime()};
}

} // namespace pomdog
