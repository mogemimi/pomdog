// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/StackPanel.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/MathHelper.hpp"

namespace Pomdog::GUI {

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

    const auto collisionHeight = barHeight + padding.Top;
    Rectangle captionBar{
        0,
        GetHeight() - collisionHeight,
        GetWidth(),
        collisionHeight};

    if (captionBar.Contains(pointInView)) {
        startTouchPoint = MathHelper::ToVector2(pointInView);
    }
}

void StackPanel::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    auto position = MathHelper::ToVector2(pointInView);

    auto tangent = position - *startTouchPoint;
    auto distanceSquared = tangent.LengthSquared();

    if (distanceSquared >= 1.4143f) {
        SetPosition(GetPosition() + MathHelper::ToPoint2D(tangent));

        // NOTE: recalculate position in current coordinate system
        pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
        position = MathHelper::ToVector2(pointInView);
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
}

std::shared_ptr<Widget> StackPanel::GetChildAt(const Point2D& position)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    auto bounds = verticalLayout->GetBounds();
    if (bounds.Contains(position)) {
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

    const auto requiredHeight = padding.Top + barHeight + verticalLayout->GetHeight() + padding.Bottom;
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
        verticalLayout->SetPosition(Point2D{padding.Left, padding.Bottom});

        switch (verticalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (padding.Left + padding.Right);
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

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const Color backgroundColor = {45, 45, 48, 225};
    const Color borderColor = {40, 40, 40, 255};
    const Color highlightColor = {106, 106, 106, 255};

    const auto w = static_cast<float>(GetWidth());
    const auto h = static_cast<float>(GetHeight());

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        MathHelper::ToVector2(globalPos),
        w,
        h,
        backgroundColor);

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        Vector2{0, h - barHeight} + MathHelper::ToVector2(globalPos),
        w,
        static_cast<float>(barHeight),
        highlightColor);

    const auto pos = MathHelper::ToVector2(globalPos);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{w, 0.0f}, borderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{0.0f, h}, borderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{0.0f, h}, pos + Vector2{w, h}, borderColor, 1.0f);
    primitiveBatch->DrawLine(pos + Vector2{w, 0.0f}, pos + Vector2{w, h}, borderColor, 1.0f);
    primitiveBatch->Flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
