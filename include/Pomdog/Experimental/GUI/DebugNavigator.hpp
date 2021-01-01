// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

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

} // namespace Pomdog::GUI
