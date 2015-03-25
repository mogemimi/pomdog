// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "UIView.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
UIView::UIView(Matrix3x2 const& transformIn, std::uint32_t widthIn, std::uint32_t heightIn)
	: transform(transformIn)
	, parentTransform(Matrix3x2::Identity)
	, drawOrder(0)
	, parentDrawOrder(0)
	, width(widthIn)
	, height(heightIn)
	, isParentTransformDirty(true)
	, isParentDrawOrderDirty(true)
{
}
//-----------------------------------------------------------------------
Matrix3x2 UIView::Transform() const {
	return transform;
}
//-----------------------------------------------------------------------
void UIView::Transform(Matrix3x2 const& transformMatrixIn) {
	this->transform = transformMatrixIn;
}
//-----------------------------------------------------------------------
void UIView::Transform(Matrix3x2 && transformMatrixIn) {
	this->transform = std::move(transformMatrixIn);
}
//-----------------------------------------------------------------------
std::weak_ptr<UIElement const> UIView::Parent() const
{
	return parent;
}
//-----------------------------------------------------------------------
std::weak_ptr<UIElement> UIView::Parent()
{
	return parent;
}
//-----------------------------------------------------------------------
void UIView::Parent(std::weak_ptr<UIElement> const& parentIn)
{
	this->parent = parentIn;
}
//-----------------------------------------------------------------------
void UIView::UpdateTransform()
{
	if (isParentTransformDirty)
	{
		if (auto element = parent.lock()) {
			parentTransform = element->GlobalTransform();
		}
		isParentTransformDirty = false;
	}
}
//-----------------------------------------------------------------------
void UIView::MarkParentTransformDirty()
{
	this->isParentTransformDirty = true;
}
//-----------------------------------------------------------------------
Matrix3x2 UIView::GlobalTransform() const
{
	POMDOG_ASSERT(!isParentTransformDirty);
	return transform * parentTransform;
}
//-----------------------------------------------------------------------
void UIView::MarkParentDrawOrderDirty()
{
	this->isParentDrawOrderDirty = true;
}
//-----------------------------------------------------------------------
std::int32_t UIView::GlobalDrawOrder()
{
	if (isParentDrawOrderDirty)
	{
		if (auto element = parent.lock()) {
			parentDrawOrder = element->GlobalDrawOrder() - 1;
		}
		isParentDrawOrderDirty = false;
	}

	return drawOrder + parentDrawOrder;
}
//-----------------------------------------------------------------------
void UIView::DrawOrder(std::int32_t drawOrderIn)
{
	this->drawOrder = drawOrderIn;
}
//-----------------------------------------------------------------------
std::int32_t UIView::DrawOrder() const
{
	return drawOrder;
}
//-----------------------------------------------------------------------
void UIView::SetCursor(MouseCursor cursorIn)
{
	cursor = cursorIn;
}
//-----------------------------------------------------------------------
void UIView::ResetCursor()
{
	cursor = OptionalType::NullOptional;
}
//-----------------------------------------------------------------------
Optional<MouseCursor> UIView::CurrentCursor() const
{
	return cursor;
}
//-----------------------------------------------------------------------
Rectangle UIView::BoundingBox() const
{
	return Rectangle{0, 0, width, height};
}
//-----------------------------------------------------------------------
bool UIView::SizeToFitContent() const
{
	return false;
}
//-----------------------------------------------------------------------
std::weak_ptr<UIEventDispatcher> UIView::Dispatcher() const
{
	POMDOG_ASSERT(false);
	return {};
}
//-----------------------------------------------------------------------
void UIView::OnParentChanged() {}
//-----------------------------------------------------------------------
void UIView::OnPointerCanceled(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerCaptureLost(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerWheelChanged(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerEntered(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerExited(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerPressed(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerMoved(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnPointerReleased(PointerPoint const&) {}
//-----------------------------------------------------------------------
void UIView::OnRenderSizeChanged(std::uint32_t, std::uint32_t) {}
//-----------------------------------------------------------------------
void UIView::Draw(DrawingContext &) {}
//-----------------------------------------------------------------------
void UIView::UpdateAnimation(Duration const&) {}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
