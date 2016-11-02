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
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        std::uint32_t widthIn,
        std::uint32_t heightIn);

    bool SizeToFitContent() const override { return false; }

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

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
