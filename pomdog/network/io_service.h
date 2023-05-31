// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/signals/signal.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT IOService final {
private:
    std::shared_ptr<GameClock> clock_;
    Signal<void()> tasks_;

public:
    IOService() noexcept;

    IOService(const IOService&) = delete;
    IOService& operator=(const IOService&) = delete;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameClock>& clock);

    [[nodiscard]] std::unique_ptr<Error>
    shutdown();

    void step();

    [[nodiscard]] Connection
    scheduleTask(std::function<void()>&& func);

    [[nodiscard]] TimePoint
    getNowTime() const;
};

} // namespace pomdog
