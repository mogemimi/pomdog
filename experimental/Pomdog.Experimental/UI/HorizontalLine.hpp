// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"

namespace Pomdog {
namespace UI {

class HorizontalLine final
    : public UIElement
    , public std::enable_shared_from_this<HorizontalLine> {
public:
    explicit HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext & drawingContext) override;

private:
    Rendering::PrimitiveFunctionCommand renderCommand;
};

} // namespace UI
} // namespace Pomdog
