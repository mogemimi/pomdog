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
	: renderTransform(transformIn)
	, parentTransform(Matrix3x2::Identity)
	, width(widthIn)
	, height(heightIn)
	, isParentTransformDirty(true)
{
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
void UIView::MarkParentTransformDirty()
{
	this->isParentTransformDirty = true;
}
//-----------------------------------------------------------------------
Matrix3x2 UIView::GlobalTransform()
{
	if (isParentTransformDirty)
	{
		if (auto element = parent.lock()) {
			parentTransform = element->GlobalTransform();
		}
		isParentTransformDirty = false;
	}
	
	return renderTransform * parentTransform;
}

}// namespace UI
}// namespace Pomdog
