// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "Thickness.hpp"
#include "detail/UIEventConnection.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <list>

namespace Pomdog {
namespace UI {

class StackPanel: public UIElement, public std::enable_shared_from_this<StackPanel> {
public:
    StackPanel(
        std::shared_ptr<UIEventDispatcher> const& dispatcher,
        std::uint32_t widthIn,
        std::uint32_t heightIn);

    bool SizeToFitContent() const override { return false; }

    void OnEnter() override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerMoved(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(Duration const& frameDuration) override;

    void AddChild(std::shared_ptr<UIElement> const& element);

    void UpdateTransform() override final;

private:
    using UIElementCollection = std::list<std::shared_ptr<UIElement>>;
    UIElementCollection children;
    Detail::UIEventConnection connection;
    Thickness padding;
    std::uint16_t barHeight;
    Optional<Vector2> startTouchPoint;
};

} // namespace UI
} // namespace Pomdog
