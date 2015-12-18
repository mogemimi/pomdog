// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Control.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <deque>

namespace Pomdog {
namespace UI {

class DebugNavigator: public Control {
public:
    DebugNavigator(std::shared_ptr<GameClock> const& clock);

    void Draw(DrawingContext & drawingContext) override;

private:
    std::shared_ptr<GameClock> clock;
    std::deque<float> frameRates;
    Duration duration;
    std::string frameRateString;
};

}// namespace UI
}// namespace Pomdog
