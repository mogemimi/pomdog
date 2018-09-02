// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Async/Scheduler.hpp"

namespace Pomdog {
namespace Concurrency {

class POMDOG_EXPORT ImmediateScheduler final : public Scheduler {
public:
    void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime) override;
};

} // namespace Concurrency
} // namespace Pomdog
