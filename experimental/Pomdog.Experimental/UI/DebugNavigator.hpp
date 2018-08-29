// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
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
    Rendering::PrimitiveFunctionCommand renderCommand;
    Rendering::SpriteFontCommand spriteCommand;
    Duration duration;
    std::string frameRateString;
};

} // namespace UI
} // namespace Pomdog
