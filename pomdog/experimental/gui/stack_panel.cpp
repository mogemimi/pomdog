// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

StackPanel::StackPanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding{12, 8, 10, 8}
    , barHeight(16)
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);

    verticalLayout = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout->SetStackedLayout(true);
    verticalLayout->SetLayoutSpacing(12);
}

void StackPanel::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->MarkParentTransformDirty();
}

void StackPanel::SetPadding(const Thickness& paddingIn)
{
    padding = paddingIn;
    needToUpdateLayout = true;
}

void StackPanel::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->MarkParentTransformDirty();
}

void StackPanel::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool StackPanel::GetSizeToFitContent() const noexcept
{
    return false;
}

void StackPanel::OnEnter()
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    verticalLayout->SetParent(shared_from_this());
    verticalLayout->OnEnter();
}

void StackPanel::OnPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && (*pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed)) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());

    const auto collisionHeight = barHeight + padding.top;
    Rectangle captionBar{
        0,
        GetHeight() - collisionHeight,
        GetWidth(),
        collisionHeight};

    if (captionBar.contains(pointInView)) {
        startTouchPoint = math::toVector2(pointInView);
    }
}

void StackPanel::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    auto position = math::toVector2(pointInView);

    auto tangent = position - *startTouchPoint;
    auto distanceSquared = math::lengthSquared(tangent);

    if (distanceSquared >= 1.4143f) {
        SetPosition(GetPosition() + math::toPoint2D(tangent));

        // NOTE: recalculate position in current coordinate system
        pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
        position = math::toVector2(pointInView);
        startTouchPoint = position;
    }
}

void StackPanel::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    startTouchPoint = std::nullopt;
}

void StackPanel::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->AddChild(widget);
    needToUpdateLayout = true;
}

std::shared_ptr<Widget> StackPanel::GetChildAt(const Point2D& position)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    auto bounds = verticalLayout->GetBounds();
    if (bounds.contains(position)) {
        return verticalLayout;
    }
    return nullptr;
}

void StackPanel::UpdateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->UpdateAnimation(frameDuration);
}

void StackPanel::UpdateLayout()
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->DoLayout();

    if (!needToUpdateLayout) {
        return;
    }

    const auto requiredHeight = padding.top + barHeight + verticalLayout->GetHeight() + padding.bottom;
    if (requiredHeight != GetHeight()) {
        // NOTE: Keeping the original position
        const auto positionOffset = Point2D{0, GetHeight() - requiredHeight};
        SetPosition(GetPosition() + positionOffset);

        // NOTE: Resizing this panel
        SetSize(GetWidth(), requiredHeight);

        auto parent = GetParent();
        if (parent) {
            parent->MarkContentLayoutDirty();
        }
    }

    // NOTE: Update layout for children
    {
        verticalLayout->SetPosition(Point2D{padding.left, padding.bottom});

        switch (verticalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (padding.left + padding.right);
            verticalLayout->SetSize(childWidth, verticalLayout->GetHeight());
            verticalLayout->MarkContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Left:
            break;
        case HorizontalAlignment::Right:
            break;
        }
    }

    needToUpdateLayout = false;
}

void StackPanel::DoLayout()
{
    UpdateLayout();
}

void StackPanel::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const auto w = static_cast<float>(GetWidth());
    const auto h = static_cast<float>(GetHeight());

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.x, globalPos.y, GetWidth(), GetHeight()},
        colorScheme->PanelBackgroundColor);

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.x, globalPos.y + (GetHeight() - barHeight), GetWidth(), barHeight},
        colorScheme->PanelTitleBarColor);

    const auto pos = math::toVector2(globalPos);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{w, 0.0f}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{0.0f, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, h}, pos + Vector2{w, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{w, 0.0f}, pos + Vector2{w, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->Flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace pomdog::gui
