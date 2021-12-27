// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/experimental/gui/widget.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class DebugNavigator final : public Widget {
public:
    DebugNavigator(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        const std::shared_ptr<GameClock>& clock);

    void Draw(DrawingContext& drawingContext) override;

private:
    std::shared_ptr<GameClock> clock;
    std::deque<float> frameRates;
    Duration duration;
    std::string frameRateString;
};

} // namespace pomdog::gui
