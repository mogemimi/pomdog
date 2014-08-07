//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "UIView.hpp"

namespace Pomdog {
namespace UI {

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
}// namespace UI
}// namespace Pomdog
