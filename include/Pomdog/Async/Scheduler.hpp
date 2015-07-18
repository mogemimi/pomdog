// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCHEDULER_32A1C2D2_HPP
#define POMDOG_SCHEDULER_32A1C2D2_HPP

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>
#include <chrono>

namespace Pomdog {
namespace Concurrency {

class POMDOG_EXPORT Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void Schedule(std::function<void()> && task) = 0;

    virtual void Schedule(std::function<void()> const& task) = 0;

    virtual void Schedule(std::function<void()> && task, Duration const& delay) = 0;

    virtual void Schedule(std::function<void()> const& task, Duration const& delay) = 0;
};

class POMDOG_EXPORT TickedScheduler final : public Scheduler {
public:
    TickedScheduler();

    ~TickedScheduler();

    void Schedule(std::function<void()> && task) override;

    void Schedule(std::function<void()> const& task) override;

    void Schedule(std::function<void()> && task, Duration const& delay) override;

    void Schedule(std::function<void()> const& task, Duration const& delay) override;

    void Tick();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class POMDOG_EXPORT ImmediateExecutor final : public Scheduler {
public:
    void Schedule(std::function<void()> && task) override;

    void Schedule(std::function<void()> const& task) override;

    void Schedule(std::function<void()> && task, Duration const& delay) override;

    void Schedule(std::function<void()> const& task, Duration const& delay) override;
};

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_SCHEDULER_32A1C2D2_HPP
