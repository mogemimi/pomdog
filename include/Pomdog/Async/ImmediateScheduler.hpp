// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_IMMEDIATESCHEDULER_6ABB0F4A_HPP
#define POMDOG_IMMEDIATESCHEDULER_6ABB0F4A_HPP

#include "Pomdog/Async/Scheduler.hpp"

namespace Pomdog {
namespace Concurrency {

class POMDOG_EXPORT ImmediateScheduler final : public Scheduler {
public:
    void Schedule(
        std::function<void()> && task,
        const Duration& delayTime) override;
};

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_IMMEDIATESCHEDULER_6ABB0F4A_HPP
