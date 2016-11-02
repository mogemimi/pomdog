// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "StackPanel.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

StackPanel::StackPanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : UIElement(dispatcher)
    , padding{12, 8, 10, 8}
    , barHeight(18)
{
    SetSize(widthIn, heightIn);
}

bool StackPanel::SizeToFitContent() const
{
    return false;
}

void StackPanel::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void StackPanel::OnPointerPressed(const PointerPoint& pointerPoint)
{
    Rectangle captionBar{0, 0, GetWidth(), barHeight + padding.Top};

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
    if (!captionBar.Contains(pointInView)) {
        return;
    }

    startTouchPoint = Vector2(pointInView.X, pointInView.Y);
}

void StackPanel::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
    auto position = Vector2(pointInView.X, pointInView.Y);

    auto tangent = position - *startTouchPoint;
    auto distanceSquared = tangent.LengthSquared();

    if (distanceSquared >= 1.4143f) {
        SetTransform(GetTransform() * Matrix3x2::CreateTranslation(tangent));

        ///@note recalculate position in current coordinate system
        pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
        position = Vector2(pointInView.X, pointInView.Y);
        startTouchPoint = position;

        for (auto & child : children) {
            POMDOG_ASSERT(child);
            child->MarkParentTransformDirty();
        }
    }
}

void StackPanel::OnPointerReleased(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    startTouchPoint = Pomdog::NullOpt;
}

void StackPanel::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}

void StackPanel::AddChild(const std::shared_ptr<UIElement>& element)
{
    POMDOG_ASSERT(!element->GetParent());

    Vector2 position(padding.Left, padding.Top + barHeight);

    constexpr float innerMarginBottom = 14.0f;
    for (auto & child : children) {
        position.Y += child->GetHeight();
        position.Y += innerMarginBottom;
    }

    children.push_back(element);

    element->MarkParentDrawOrderDirty();
    element->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    element->SetParent(shared_from_this());
    element->OnEnter();

    element->SetTransform(Matrix3x2::CreateTranslation(position));
    switch (element->GetHorizontalAlignment()) {
    case HorizontalAlignment::Stretch: {
        auto childWidth = GetWidth() - (padding.Left + padding.Right);
        element->SetSize(childWidth, element->GetHeight());
        break;
    }
    case HorizontalAlignment::Left:
        break;
    }

    position.Y += element->GetHeight();
    position.Y += padding.Bottom;

    if (position.Y > GetHeight()) {
        SetSize(GetWidth(), position.Y);
    }
}

void StackPanel::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    drawingContext.DrawRectangle(transform, Color{45, 45, 48, 225},
        Rectangle(0, 0, GetWidth(), GetHeight()));

    Color const borderColor{40, 40, 40, 255};

    drawingContext.DrawLine(transform, borderColor, 1.0f, {0.0f, 0.0f}, {static_cast<float>(GetWidth()), 0.0f});
    drawingContext.DrawLine(transform, borderColor, 1.0f, {0.0f, 0.0f}, {0.0f, static_cast<float>(GetHeight())});
    drawingContext.DrawLine(transform, borderColor, 1.0f, Vector2(0.0f, GetHeight()), Vector2(GetWidth(), GetHeight()));
    drawingContext.DrawLine(transform, borderColor, 1.0f, Vector2(GetWidth(), 0.0f), Vector2(GetWidth(), GetHeight()));

    Color const highlightColor{106, 106, 106, 255};
    drawingContext.DrawLine(transform, highlightColor, 1.0f, {1.0f, 1.0f}, {static_cast<float>(GetWidth()) - 1.0f, 1.0f});

    drawingContext.Push(transform);

    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.Pop();
}

void StackPanel::UpdateAnimation(const Duration& frameDuration)
{
}

void StackPanel::UpdateTransform()
{
    UIElement::UpdateTransform();

    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->UpdateTransform();
    }
}

} // namespace UI
} // namespace Pomdog
