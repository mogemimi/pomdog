// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/StackPanel.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"

namespace Pomdog::UI {

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
    verticalLayout->SetLayoutSpacing(12);
}

void StackPanel::SetTransform(const Matrix3x2& transformMatrixIn)
{
    UIElement::SetTransform(transformMatrixIn);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->MarkParentTransformDirty();
}

void StackPanel::SetPadding(const Thickness& paddingIn)
{
    padding = paddingIn;
    needToUpdateLayout = true;
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
        auto& child = verticalLayout;
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
    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());

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

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
    auto position = MathHelper::ToVector2(pointInView);

    auto tangent = position - *startTouchPoint;
    auto distanceSquared = tangent.LengthSquared();

    if (distanceSquared >= 1.4143f) {
        SetTransform(GetTransform() * Matrix3x2::CreateTranslation(tangent));

        // NOTE: recalculate position in current coordinate system
        pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
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
            static_cast<float>(padding.Bottom)};
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

void StackPanel::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto globalTransform = GetTransform() * drawingContext.Top();

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const Color backgroundColor = {45, 45, 48, 225};
    const Color borderColor = {40, 40, 40, 255};
    const Color highlightColor = {106, 106, 106, 255};

    const auto w = static_cast<float>(GetWidth());
    const auto h = static_cast<float>(GetHeight());

    primitiveBatch->DrawRectangle(globalTransform, Vector2::Zero, w, h, backgroundColor);

    primitiveBatch->DrawRectangle(
        globalTransform,
        Vector2{0, h - barHeight},
        w,
        static_cast<float>(barHeight),
        highlightColor);

    primitiveBatch->DrawLine(globalTransform, Vector2{0.0f, 0.0f}, Vector2{w, 0.0f}, borderColor, 1.0f);
    primitiveBatch->DrawLine(globalTransform, Vector2{0.0f, 0.0f}, Vector2{0.0f, h}, borderColor, 1.0f);
    primitiveBatch->DrawLine(globalTransform, Vector2{0.0f, h}, Vector2{w, h}, borderColor, 1.0f);
    primitiveBatch->DrawLine(globalTransform, Vector2{w, 0.0f}, Vector2{w, h}, borderColor, 1.0f);
    primitiveBatch->Flush();

    drawingContext.Push(globalTransform);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->Draw(drawingContext);

    drawingContext.Pop();
}

void StackPanel::UpdateAnimation([[maybe_unused]] const Duration& frameDuration)
{
}

} // namespace Pomdog::UI
