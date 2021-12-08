// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/async/scheduler.hpp"

namespace pomdog::concurrency {

class POMDOG_EXPORT ImmediateScheduler final : public Scheduler {
public:
    void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime) override;
};

} // namespace pomdog::concurrency
