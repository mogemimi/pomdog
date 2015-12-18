// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_DEBUGNAVIGATOR_9FDE3D88_HPP
#define POMDOG_DEBUGNAVIGATOR_9FDE3D88_HPP

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

#endif // POMDOG_DEBUGNAVIGATOR_9FDE3D88_HPP
