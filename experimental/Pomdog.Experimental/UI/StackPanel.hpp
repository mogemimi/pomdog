// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/Thickness.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <list>

namespace Pomdog {
namespace UI {

class StackPanel final
    : public UIElement
    , public std::enable_shared_from_this<StackPanel> {
public:
    StackPanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    bool SizeToFitContent() const override;

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
    int barHeight;
    Optional<Vector2> startTouchPoint;
};

} // namespace UI
} // namespace Pomdog
