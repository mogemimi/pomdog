// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/async/scheduler.h"

namespace pomdog::concurrency {

class POMDOG_EXPORT ImmediateScheduler final : public Scheduler {
public:
    ImmediateScheduler();
    ImmediateScheduler(const ImmediateScheduler&) = delete;
    ImmediateScheduler& operator=(const ImmediateScheduler&) = delete;

    void schedule(
        std::function<void()>&& task,
        const Duration& delayTime) override;
};

} // namespace pomdog::concurrency
