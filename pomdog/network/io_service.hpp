// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_clock.hpp"
#include "pomdog/application/time_point.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/signal.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog
