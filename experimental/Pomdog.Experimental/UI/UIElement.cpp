// Copyright (c) 2013-2016 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "UIElement.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
int UIElement::Width() const { return width; }
int UIElement::Height() const { return height; }
//-----------------------------------------------------------------------
UIElement::UIElement(std::shared_ptr<UIEventDispatcher> const& dispatcherIn)
    : parentTransform(Matrix3x2::Identity)
    , transform(Matrix3x2::Identity)
    , weakDispatcher(dispatcherIn)
    , localDrawOrder(0)
    , parentDrawOrder(0)
    , width(1)
    , height(1)
    , isParentDrawOrderDirty(true)
    , isParentTransformDirty(true)
{
}
//-----------------------------------------------------------------------
std::shared_ptr<UIEventDispatcher> UIElement::Dispatcher() const
{
    return weakDispatcher.lock();
}
//-----------------------------------------------------------------------
void UIElement::Parent(std::shared_ptr<UIElement> const& parentIn)
{
    this->weakParent = parentIn;
    POMDOG_ASSERT(this->weakDispatcher.lock() == parentIn->Dispatcher());
}
//-----------------------------------------------------------------------
std::shared_ptr<UIElement const> UIElement::Parent() const
{
    return weakParent.lock();
}
//-----------------------------------------------------------------------
std::shared_ptr<UIElement> UIElement::Parent()
{
    return weakParent.lock();
}
//-----------------------------------------------------------------------
void UIElement::SetSize(int widthIn, int heightIn)
{
    this->width = widthIn;
    this->height = heightIn;
}
//-----------------------------------------------------------------------
Rectangle UIElement::Bounds() const
{
    return Rectangle{0, 0, width, height};
}
//-----------------------------------------------------------------------
void UIElement::MarkParentDrawOrderDirty()
{
    this->isParentDrawOrderDirty = true;
}
//-----------------------------------------------------------------------
int UIElement::GlobalDrawOrder()
{
    if (isParentDrawOrderDirty) {
        if (auto element = weakParent.lock()) {
            parentDrawOrder = element->GlobalDrawOrder() - 1;
        }
        isParentDrawOrderDirty = false;
    }

    return localDrawOrder + parentDrawOrder;
}
//-----------------------------------------------------------------------
void UIElement::UpdateTransform()
{
    if (isParentTransformDirty)
    {
        if (auto element = weakParent.lock()) {
            parentTransform = element->GlobalTransform();
        } else {
            parentTransform = Matrix3x2::Identity;
        }
        isParentTransformDirty = false;
    }
}
//-----------------------------------------------------------------------
void UIElement::MarkParentTransformDirty()
{
    this->isParentTransformDirty = true;
}
//-----------------------------------------------------------------------
Matrix3x2 UIElement::GlobalTransform() const
{
    POMDOG_ASSERT(!isParentTransformDirty);
    return transform * parentTransform;
}
//-----------------------------------------------------------------------
void UIElement::DrawOrder(int drawOrderIn)
{
    this->localDrawOrder = drawOrderIn;
}
//-----------------------------------------------------------------------
int UIElement::DrawOrder() const
{
    return localDrawOrder;
}
//-----------------------------------------------------------------------
Matrix3x2 UIElement::Transform() const
{
    return transform;
}
//-----------------------------------------------------------------------
void UIElement::Transform(Matrix3x2 const& transformMatrixIn)
{
    this->transform = transformMatrixIn;
}
//-----------------------------------------------------------------------
void UIElement::Transform(Matrix3x2 && transformMatrixIn)
{
    this->transform = std::move(transformMatrixIn);
}
//-----------------------------------------------------------------------
void UIElement::OnEnter() {}
//-----------------------------------------------------------------------
void UIElement::OnPointerCanceled(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerCaptureLost(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerWheelChanged(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerEntered(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerExited(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerPressed(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerMoved(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnPointerReleased(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIElement::OnRenderSizeChanged(int, int) {}
//-----------------------------------------------------------------------
void UIElement::Draw(DrawingContext &) {}
//-----------------------------------------------------------------------
void UIElement::UpdateAnimation(Duration const&) {}
//-----------------------------------------------------------------------
void UIElement::SetCursor(MouseCursor cursorIn)
{
    cursor = cursorIn;
}
//-----------------------------------------------------------------------
void UIElement::ResetCursor()
{
    cursor = Pomdog::NullOpt;
}
//-----------------------------------------------------------------------
Optional<MouseCursor> UIElement::CurrentCursor() const
{
    return cursor;
}
//-----------------------------------------------------------------------
} // namespace UI
} // namespace Pomdog
