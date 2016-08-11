// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "StackPanel.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

StackPanel::StackPanel(
    std::shared_ptr<UIEventDispatcher> const& dispatcher,
    std::uint32_t widthIn,
    std::uint32_t heightIn)
    : UIElement(dispatcher)
    , padding{12, 8, 10, 8}
    , barHeight(18)
{
    SetSize(widthIn, heightIn);
}

void StackPanel::OnEnter()
{
    auto dispatcher = Dispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void StackPanel::OnPointerPressed(PointerPoint const& pointerPoint)
{
    Rectangle captionBar{0, 0, Width(), barHeight + padding.Top};

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
    if (!captionBar.Contains(pointInView)) {
        return;
    }

    startTouchPoint = Vector2(pointInView.X, pointInView.Y);
}

void StackPanel::OnPointerMoved(PointerPoint const& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
    auto position = Vector2(pointInView.X, pointInView.Y);

    auto tangent = position - *startTouchPoint;
    auto distanceSquared = tangent.LengthSquared();

    if (distanceSquared >= 1.4143f)
    {
        Transform(Transform() * Matrix3x2::CreateTranslation(tangent));

        ///@note recalculate position in current coordinate system
        pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
        position = Vector2(pointInView.X, pointInView.Y);
        startTouchPoint = position;

        for (auto & child: children)
        {
            POMDOG_ASSERT(child);
            child->MarkParentTransformDirty();
        }
    }
}

void StackPanel::OnPointerReleased(PointerPoint const& pointerPoint)
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

void StackPanel::AddChild(std::shared_ptr<UIElement> const& element)
{
    POMDOG_ASSERT(!element->Parent());

    Vector2 position(padding.Left, padding.Top + barHeight);

    constexpr float innerMarginBottom = 14.0f;
    for (auto & child: children)
    {
        position.Y += child->Height();
        position.Y += innerMarginBottom;
    }

    children.push_back(element);

    element->MarkParentDrawOrderDirty();
    element->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    element->Parent(shared_from_this());
    element->OnEnter();

    element->Transform(Matrix3x2::CreateTranslation(position));
    switch (element->HorizontalAlignment()) {
    case HorizontalAlignment::Stretch: {
        auto childWidth = Width() - (padding.Left + padding.Right);
        element->SetSize(childWidth, element->Height());
        break;
    }
    case HorizontalAlignment::Left:
        break;
    }

    position.Y += element->Height();
    position.Y += padding.Bottom;

    if (position.Y > Height())
    {
        SetSize(Width(), position.Y);
    }
}

void StackPanel::Draw(DrawingContext & drawingContext)
{
    auto transform = Transform() * drawingContext.Top();

    drawingContext.DrawRectangle(transform, Color{45, 45, 48, 225},
        Rectangle(0, 0, Width(), Height()));

    Color const borderColor{40, 40, 40, 255};

    drawingContext.DrawLine(transform, borderColor, 1.0f, {0.0f, 0.0f}, {static_cast<float>(Width()), 0.0f});
    drawingContext.DrawLine(transform, borderColor, 1.0f, {0.0f, 0.0f}, {0.0f, static_cast<float>(Height())});
    drawingContext.DrawLine(transform, borderColor, 1.0f, Vector2(0.0f, Height()), Vector2(Width(), Height()));
    drawingContext.DrawLine(transform, borderColor, 1.0f, Vector2(Width(), 0.0f), Vector2(Width(), Height()));

    Color const highlightColor{106, 106, 106, 255};
    drawingContext.DrawLine(transform, highlightColor, 1.0f, {1.0f, 1.0f}, {static_cast<float>(Width()) - 1.0f, 1.0f});

    drawingContext.Push(transform);

    for (auto & child: children)
    {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.Pop();
}

void StackPanel::UpdateAnimation(Duration const& frameDuration)
{
}

void StackPanel::UpdateTransform()
{
    UIElement::UpdateTransform();

    for (auto & child: children)
    {
        POMDOG_ASSERT(child);
        child->UpdateTransform();
    }
}

} // namespace UI
} // namespace Pomdog
