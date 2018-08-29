// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/StackPanel.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

StackPanel::StackPanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : UIElement(dispatcher)
    , padding{12, 8, 10, 8}
    , barHeight(16)
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);

    verticalLayout = std::make_shared<UI::VerticalLayout>(dispatcher, 140, 10);
    verticalLayout->SetStackedLayout(true);
    verticalLayout->SetLayoutSpacing(8);
}

void StackPanel::SetTransform(const Matrix3x2& transformMatrixIn)
{
    UIElement::SetTransform(transformMatrixIn);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->MarkParentTransformDirty();
}

void StackPanel::MarkParentTransformDirty()
{
    UIElement::MarkParentTransformDirty();

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->MarkParentTransformDirty();
}

void StackPanel::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool StackPanel::SizeToFitContent() const
{
    return false;
}

void StackPanel::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());

    {
        auto & child = verticalLayout;
        POMDOG_ASSERT(child);
        child->MarkParentDrawOrderDirty();
        child->MarkParentTransformDirty();

        POMDOG_ASSERT(shared_from_this());
        child->SetParent(shared_from_this());
        child->OnEnter();
    }
}

void StackPanel::OnPointerPressed(const PointerPoint& pointerPoint)
{
    const auto collisionHeight = barHeight + padding.Top;
    Rectangle captionBar{
        0,
        GetHeight() - collisionHeight,
        GetWidth(),
        collisionHeight};

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
        position = MathHelper::ToVector2(pointInView);
        startTouchPoint = position;
    }
}

void StackPanel::OnPointerReleased(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint) {
        return;
    }

    startTouchPoint = std::nullopt;
}

void StackPanel::AddChild(const std::shared_ptr<UIElement>& element)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->AddChild(element);
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
        const auto positionOffset = MathHelper::ToVector2({0, GetHeight() - requiredHeight});
        SetTransform(Matrix3x2::CreateTranslation(positionOffset) * GetTransform());

        // NOTE: Resizing this panel
        SetSize(GetWidth(), requiredHeight);

        auto parent = GetParent();
        if (parent) {
            parent->MarkContentLayoutDirty();
        }
    }

    // NOTE: Update layout for children
    {
        const Vector2 position = {
            static_cast<float>(padding.Left),
            static_cast<float>(padding.Bottom)
        };
        verticalLayout->SetTransform(Matrix3x2::CreateTranslation(position));

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

void StackPanel::Draw(DrawingContext & drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto transform = GetTransform() * drawingContext.Top();

    renderCommand.SetInvoker([transform, this](PolygonBatch & polygonBatch) {
        const Color backgroundColor = {45, 45, 48, 225};
        const Color borderColor = {40, 40, 40, 255};
        const Color highlightColor = {106, 106, 106, 255};

        const auto width = static_cast<float>(GetWidth());
        const auto height = static_cast<float>(GetHeight());

        polygonBatch.DrawRectangle(transform, Vector2::Zero, width, height, backgroundColor);

        polygonBatch.DrawLine(transform, Vector2{0.0f, 0.0f}, Vector2{width, 0.0f}, borderColor, 1.0f);
        polygonBatch.DrawLine(transform, Vector2{0.0f, 0.0f}, Vector2{0.0f, height}, borderColor, 1.0f);
        polygonBatch.DrawLine(transform, Vector2{0.0f, height}, Vector2{width, height}, borderColor, 1.0f);
        polygonBatch.DrawLine(transform, Vector2{width, 0.0f}, Vector2{width, height}, borderColor, 1.0f);

        polygonBatch.DrawRectangle(
            transform,
            Vector2{0, height - barHeight},
            width,
            barHeight,
            highlightColor);
    });
    drawingContext.PushCommand(renderCommand);

    drawingContext.Push(transform);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->Draw(drawingContext);

    drawingContext.Pop();
}

void StackPanel::UpdateAnimation(const Duration& frameDuration)
{
}

} // namespace UI
} // namespace Pomdog
