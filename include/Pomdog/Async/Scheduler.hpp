// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <functional>

namespace Pomdog {
namespace Concurrency {

class POMDOG_EXPORT Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime = Duration::zero()) = 0;
};

} // namespace Concurrency
} // namespace Pomdog
