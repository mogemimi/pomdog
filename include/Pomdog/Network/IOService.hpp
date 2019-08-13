// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/TimePoint.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <functional>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT IOService final {
public:
    explicit IOService(const GameClock* clock);

    IOService(const IOService&) = delete;
    IOService& operator=(const IOService&) = delete;

    [[nodiscard]] std::shared_ptr<Error> Initialize();

    [[nodiscard]] std::shared_ptr<Error> Shutdown();

    void Step();

    Connection ScheduleTask(std::function<void()>&& func);

    TimePoint GetNowTime() const;

private:
    const GameClock* clock = nullptr;
    Signal<void()> tasks;
};

} // namespace Pomdog
