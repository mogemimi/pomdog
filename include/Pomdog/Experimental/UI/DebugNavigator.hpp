// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Experimental/UI/UIElement.hpp"
#include <deque>
#include <memory>
#include <string>

namespace Pomdog::UI {

class DebugNavigator final : public UIElement {
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

} // namespace Pomdog::UI
