// Copyright (c) 2013-2018 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace UI {

UIElement::UIElement(const std::shared_ptr<UIEventDispatcher>& dispatcherIn)
    : transform(Matrix3x2::Identity)
    , parentTransform(Matrix3x2::Identity)
    , weakDispatcher(dispatcherIn)
    , parentDrawOrder(0)
    , localDrawOrder(0)
    , height(1)
    , width(1)
    , isParentDrawOrderDirty(true)
    , isParentTransformDirty(true)
{
}

std::shared_ptr<UIEventDispatcher> UIElement::GetDispatcher() const
{
    return weakDispatcher.lock();
}

void UIElement::SetParent(const std::shared_ptr<UIElement>& parentIn)
{
    this->weakParent = parentIn;
    POMDOG_ASSERT(this->weakDispatcher.lock() == parentIn->GetDispatcher());
}

std::shared_ptr<UIElement const> UIElement::GetParent() const
{
    return weakParent.lock();
}

std::shared_ptr<UIElement> UIElement::GetParent()
{
    return weakParent.lock();
}

int UIElement::GetWidth() const
{
    return width;
}

int UIElement::GetHeight() const
{
    return height;
}

void UIElement::SetSize(int widthIn, int heightIn)
{
    this->width = widthIn;
    this->height = heightIn;
}

Rectangle UIElement::GetBounds() const
{
    return Rectangle{0, 0, width, height};
}

void UIElement::MarkParentDrawOrderDirty()
{
    this->isParentDrawOrderDirty = true;
}

int UIElement::GetGlobalDrawOrder()
{
    if (isParentDrawOrderDirty) {
        if (auto element = weakParent.lock()) {
            parentDrawOrder = element->GetGlobalDrawOrder() - 1;
        }
        isParentDrawOrderDirty = false;
    }
    return localDrawOrder + parentDrawOrder;
}

void UIElement::MarkParentTransformDirty()
{
    this->isParentTransformDirty = true;
}

Matrix3x2 UIElement::GetGlobalTransform()
{
    if (isParentTransformDirty) {
        if (auto element = weakParent.lock()) {
            parentTransform = element->GetGlobalTransform();
        }
        else {
            parentTransform = Matrix3x2::Identity;
        }
        isParentTransformDirty = false;
    }

    POMDOG_ASSERT(!isParentTransformDirty);
    return transform * parentTransform;
}

void UIElement::SetDrawOrder(int drawOrderIn)
{
    this->localDrawOrder = drawOrderIn;
}

int UIElement::GetDrawOrder() const
{
    return localDrawOrder;
}

Matrix3x2 UIElement::GetTransform() const
{
    return transform;
}

void UIElement::SetTransform(const Matrix3x2& transformMatrixIn)
{
    this->transform = transformMatrixIn;
}

void UIElement::MarkContentLayoutDirty()
{
}

void UIElement::DoLayout()
{
}

bool UIElement::SizeToFitContent() const
{
    return false;
}

HorizontalAlignment UIElement::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Stretch;
}

VerticalAlignment UIElement::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Stretch;
}

void UIElement::OnEnter()
{
}

void UIElement::OnPointerCanceled(const PointerPoint&)
{
}

void UIElement::OnPointerCaptureLost(const PointerPoint&)
{
}

void UIElement::OnPointerWheelChanged(const PointerPoint&)
{
}

void UIElement::OnPointerEntered(const PointerPoint&)
{
}

void UIElement::OnPointerExited(const PointerPoint&)
{
}

void UIElement::OnPointerPressed(const PointerPoint&)
{
}

void UIElement::OnPointerMoved(const PointerPoint&)
{
}

void UIElement::OnPointerReleased(const PointerPoint&)
{
}

void UIElement::Draw(DrawingContext &)
{
}

void UIElement::UpdateAnimation(const Duration&)
{
}

void UIElement::SetCursor(MouseCursor cursorIn)
{
    cursor = cursorIn;
}

void UIElement::ResetCursor()
{
    cursor = std::nullopt;
}

std::optional<MouseCursor> UIElement::GetCurrentCursor() const
{
    return cursor;
}

} // namespace UI
} // namespace Pomdog
