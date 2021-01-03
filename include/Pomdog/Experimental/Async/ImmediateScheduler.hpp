// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Async/Scheduler.hpp"

namespace Pomdog::Concurrency {

class POMDOG_EXPORT ImmediateScheduler final : public Scheduler {
public:
    void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime) override;
};

} // namespace Pomdog::Concurrency
