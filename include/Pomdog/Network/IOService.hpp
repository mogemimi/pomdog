// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/TimePoint.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT IOService final {
public:
    explicit IOService(const GameClock* clock);

    IOService(const IOService&) = delete;
    IOService& operator=(const IOService&) = delete;

    [[nodiscard]] std::unique_ptr<Error> Initialize();

    [[nodiscard]] std::unique_ptr<Error> Shutdown();

    void Step();

    Connection ScheduleTask(std::function<void()>&& func);

    TimePoint GetNowTime() const;

private:
    const GameClock* clock = nullptr;
    Signal<void()> tasks;
};

} // namespace Pomdog
