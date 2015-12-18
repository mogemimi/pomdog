// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SCHEDULER_32A1C2D2_HPP
#define POMDOG_SCHEDULER_32A1C2D2_HPP

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <functional>

namespace Pomdog {
namespace Concurrency {

class POMDOG_EXPORT Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void Schedule(
        std::function<void()> && task,
        const Duration& delayTime = Duration::zero()) = 0;
};

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_SCHEDULER_32A1C2D2_HPP
