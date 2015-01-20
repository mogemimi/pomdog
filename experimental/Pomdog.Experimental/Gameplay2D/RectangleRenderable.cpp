//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "RectangleRenderable.hpp"
#include "../Rendering/Renderer.hpp"
#include <Pomdog.Experimental/Gameplay2D/Transform2D.hpp>
#include <limits>

namespace Pomdog {
namespace {

///@todo badcode: Copy and Paste
static Matrix3x2 CreateTransformMatrix3x2(Transform2D const& transform)
{
	return Matrix3x2::CreateScale(transform.Scale)
		* Matrix3x2::CreateRotation(transform.Rotation)
		* Matrix3x2::CreateTranslation(transform.Position);
}

}// namespace SpriteRenderableDirtyFlags
//-----------------------------------------------------------------------
RectangleRenderable::RectangleRenderable()
	: originPivot{0.5f, 0.5f}
	, dirtyFlags{std::numeric_limits<std::uint32_t>::max()}
{
	command.transform = Matrix3x2::Identity;
	command.leftTopColor = Color::White;
	command.rightTopColor = Color::White;
	command.leftBottomColor = Color::White;
	command.rightBottomColor = Color::White;
	command.drawOrder = 0;
}
//-----------------------------------------------------------------------
void RectangleRenderable::Visit(GameObject & gameObject, Renderer & renderer)
{
	if (!IsVisible) {
		return;
	}

	dirtyFlags = 0;

	command.drawOrder = DrawOrder;

	auto offsetMatrix = Matrix3x2::CreateTranslation(-Vector2(command.rectangle.Width, command.rectangle.Height) * originPivot);

	if (auto transform = gameObject.Component<Transform2D>())
	{
		if (transform->Scale.X == 0.0f || transform->Scale.Y == 0.0f) {
			return;
		}
		command.transform = offsetMatrix * CreateTransformMatrix3x2(*transform);
	}
	else {
		command.transform = offsetMatrix;
	}

	POMDOG_ASSERT(command.rectangle.Width > 0);
	POMDOG_ASSERT(command.rectangle.Height > 0);

	renderer.PushCommand(command);
}
//-----------------------------------------------------------------------
void RectangleRenderable::LeftTopColor(Pomdog::Color const& color)
{
	command.leftTopColor = color;
}
//-----------------------------------------------------------------------
void RectangleRenderable::RightTopColor(Pomdog::Color const& color)
{
	command.rightTopColor = color;
}
//-----------------------------------------------------------------------
void RectangleRenderable::LeftBottomColor(Pomdog::Color const& color)
{
	command.leftBottomColor = color;
}
//-----------------------------------------------------------------------
void RectangleRenderable::RightBottomColor(Pomdog::Color const& color)
{
	command.rightBottomColor = color;
}
//-----------------------------------------------------------------------
void RectangleRenderable::OriginPivot(Vector2 const& originPivotIn)
{
	this->originPivot = originPivotIn;
}
//-----------------------------------------------------------------------
Vector2 RectangleRenderable::OriginPivot() const
{
	return originPivot;
}
//-----------------------------------------------------------------------
void RectangleRenderable::BoundingBox(Rectangle const& rectangle)
{
	command.rectangle = rectangle;
}
//-----------------------------------------------------------------------
Rectangle RectangleRenderable::BoundingBox() const
{
	return command.rectangle;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
