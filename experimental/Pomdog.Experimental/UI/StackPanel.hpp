// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/Thickness.hpp"
#include "Pomdog.Experimental/UI/VerticalLayout.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <list>
#include <optional>

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

    void SetTransform(const Matrix3x2& matrix) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool SizeToFitContent() const override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout;
    Detail::UIEventConnection connection;
    Rendering::PrimitiveFunctionCommand renderCommand;
    Thickness padding;
    int barHeight;
    std::optional<Vector2> startTouchPoint;
    bool needToUpdateLayout;
};

} // namespace UI
} // namespace Pomdog
