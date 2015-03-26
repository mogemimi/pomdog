// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_STACKPANEL_1285AAF5_DFD5_44B2_974C_040829A6EB6C_HPP
#define POMDOG_STACKPANEL_1285AAF5_DFD5_44B2_974C_040829A6EB6C_HPP

#include "Panel.hpp"
#include "Thickness.hpp"
#include "detail/UIEventConnection.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <list>

namespace Pomdog {
namespace UI {

class StackPanel: public Panel, public std::enable_shared_from_this<StackPanel> {
public:
    StackPanel(std::uint32_t widthIn, std::uint32_t heightIn);

    bool SizeToFitContent() const override { return false; }

    std::weak_ptr<UIEventDispatcher> Dispatcher() const override { return weakDispatcher; }

    void OnParentChanged() override;

    void OnPointerCanceled(PointerPoint const& pointerPoint) override;

    void OnPointerCaptureLost(PointerPoint const& pointerPoint) override;

    void OnPointerEntered(PointerPoint const& pointerPoint) override;

    void OnPointerExited(PointerPoint const& pointerPoint) override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerMoved(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(Duration const& frameDuration) override;

    void AddChild(std::shared_ptr<UIView> const& element);

    void UpdateTransform() override final;

private:
    using UIElementCollection = std::list<std::shared_ptr<UIElement>>;
    UIElementCollection children;

    std::weak_ptr<UIEventDispatcher> weakDispatcher;
    Detail::UIEventConnection connection;

    Thickness padding;
    std::uint16_t barHeight;

    Optional<Vector2> startTouchPoint;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_STACKPANEL_1285AAF5_DFD5_44B2_974C_040829A6EB6C_HPP)
