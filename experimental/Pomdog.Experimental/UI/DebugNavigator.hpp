// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <deque>

namespace Pomdog {
namespace UI {

class DebugNavigator final : public UIElement {
public:
    DebugNavigator(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        const std::shared_ptr<GameClock>& clock);

    void Draw(DrawingContext & drawingContext) override;

private:
    std::shared_ptr<GameClock> clock;
    std::deque<float> frameRates;
    Duration duration;
    std::string frameRateString;
};

} // namespace UI
} // namespace Pomdog
