// Copyright (c) 2013-2019 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::GUI {

Widget::Widget(const std::shared_ptr<UIEventDispatcher>& dispatcherIn)
    : localPosition(0, 0)
    , parentPosition(0, 0)
    , weakDispatcher(dispatcherIn)
    , height(1)
    , width(1)
    , isParentTransformDirty(true)
    , isVisible(true)
    , isInteractable(true)
    , isWheelFocusEnabled(false)
{
}

std::shared_ptr<UIEventDispatcher> Widget::GetDispatcher() const
{
    return weakDispatcher.lock();
}

void Widget::SetParent(const std::shared_ptr<Widget>& parentIn)
{
    this->weakParent = parentIn;
    POMDOG_ASSERT(this->weakDispatcher.lock() == parentIn->GetDispatcher());
}

std::shared_ptr<Widget const> Widget::GetParent() const
{
    return weakParent.lock();
}

std::shared_ptr<Widget> Widget::GetParent()
{
    return weakParent.lock();
}

int Widget::GetX() const noexcept
{
    return localPosition.X;
}

int Widget::GetY() const noexcept
{
    return localPosition.Y;
}

int Widget::GetWidth() const noexcept
{
    return width;
}

int Widget::GetHeight() const noexcept
{
    return height;
}

void Widget::SetSize(int widthIn, int heightIn)
{
    this->width = widthIn;
    this->height = heightIn;
}

Rectangle Widget::GetBounds() const noexcept
{
    return Rectangle{ localPosition.X, localPosition.Y, width, height};
}

void Widget::MarkParentTransformDirty()
{
    this->isParentTransformDirty = true;
}

Point2D Widget::GetGlobalPosition()
{
    if (isParentTransformDirty) {
        if (auto widget = weakParent.lock(); widget != nullptr) {
            parentPosition = widget->GetGlobalPosition();
        }
        else {
            parentPosition = Point2D{0, 0};
        }
        isParentTransformDirty = false;
    }

    POMDOG_ASSERT(!isParentTransformDirty);
    return localPosition + parentPosition;
}

bool Widget::IsVisible() const noexcept
{
    return isVisible;
}

void Widget::SetVisible(bool visible) noexcept
{
    this->isVisible = visible;
}

bool Widget::IsInteractable() const noexcept
{
    return isInteractable;
}

void Widget::SetInteractable(bool interactable) noexcept
{
    this->isInteractable = interactable;
}

bool Widget::IsWheelFocusEnabled() const noexcept
{
    return isWheelFocusEnabled;
}

void Widget::SetWheelFocusEnabled(bool wheelFocusEnabled) noexcept
{
    this->isWheelFocusEnabled = wheelFocusEnabled;
}

Point2D Widget::GetPosition() const noexcept
{
    return localPosition;
}

void Widget::SetPosition(const Point2D& positionIn)
{
    this->localPosition = positionIn;
}

void Widget::MarkContentLayoutDirty()
{
}

void Widget::DoLayout()
{
}

std::shared_ptr<Widget> Widget::GetChildAt([[maybe_unused]] const Point2D& positionIn)
{
    return nullptr;
}

bool Widget::SizeToFitContent() const
{
    return false;
}

HorizontalAlignment Widget::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment Widget::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Stretch;
}

void Widget::OnEnter()
{
}

void Widget::OnFocusIn()
{
}

void Widget::OnFocusOut()
{
}

void Widget::OnTextInput(const KeyboardState&, const std::string&)
{
}

void Widget::OnKeyDown(const KeyboardState&, Keys)
{
}

void Widget::OnKeyUp(const KeyboardState&, Keys)
{
}

void Widget::OnPointerCanceled(const PointerPoint&)
{
}

void Widget::OnPointerCaptureLost(const PointerPoint&)
{
}

void Widget::OnPointerWheelChanged(const PointerPoint&)
{
}

void Widget::OnPointerEntered(const PointerPoint&)
{
}

void Widget::OnPointerExited(const PointerPoint&)
{
}

void Widget::OnPointerPressed(const PointerPoint&)
{
}

void Widget::OnPointerMoved(const PointerPoint&)
{
}

void Widget::OnPointerReleased(const PointerPoint&)
{
}

void Widget::Draw(DrawingContext&)
{
}

void Widget::UpdateAnimation(const Duration&)
{
}

void Widget::SetCursor(MouseCursor cursorIn)
{
    cursor = cursorIn;
}

void Widget::ResetCursor()
{
    cursor = std::nullopt;
}

std::optional<MouseCursor> Widget::GetCurrentCursor() const
{
    return cursor;
}

} // namespace Pomdog::GUI
