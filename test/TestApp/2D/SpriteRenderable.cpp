//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteRenderable.hpp"
#include "../Rendering/Renderer.hpp"

namespace Pomdog {
namespace {

static Matrix3x2 CreateTransformMatrix3x2(Transform2D const& transform)
{
	return Matrix3x2::CreateScale(transform.Scale)
		* Matrix3x2::CreateRotation(transform.Rotation)
		* Matrix3x2::CreateTranslation(transform.Position);
}
//-----------------------------------------------------------------------
static TextureRegion CreateTextureRegionFromTexture(std::shared_ptr<Texture2D> const& texture)
{
	POMDOG_ASSERT(texture);
	TextureRegion textureRegion;
	textureRegion.Subrect.X = 0;
	textureRegion.Subrect.Y = 0;
	textureRegion.Subrect.Width = texture->Width();
	textureRegion.Subrect.Height = texture->Height();
	textureRegion.XOffset = 0;
	textureRegion.YOffset = 0;
	textureRegion.Width = texture->Width();
	textureRegion.Height = texture->Height();
	textureRegion.Rotate = false;
	textureRegion.Flip = false;
	return std::move(textureRegion);
}

}// unnamed namespace
//-----------------------------------------------------------------------
SpriteRenderable::SpriteRenderable(std::shared_ptr<Texture2D> const& texture)
	: SpriteRenderable(texture, CreateTextureRegionFromTexture(texture))
{}
//-----------------------------------------------------------------------
SpriteRenderable::SpriteRenderable(std::shared_ptr<Texture2D> const& textureIn, TextureRegion const& textureRegionIn)
	: zOrder(0)
	, isVisible(true)
{
	POMDOG_ASSERT(textureIn);
	command.texture = textureIn;
	command.textureRegion = textureRegionIn;
	command.transform = Matrix3x2::Identity;
	command.color = Color::White;
	command.origin = {0.5f, 0.5f};
	command.zOrder = 0;
}
//-----------------------------------------------------------------------
void SpriteRenderable::Visit(GameObject & gameObject, Renderer & renderer,
	Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix)
{
	if (!isVisible) {
		return;
	}
	
	command.zOrder = zOrder;

	if (auto transform = gameObject.Component<Transform2D>())
	{
		if (transform->Scale.X == 0.0f || transform->Scale.Y == 0.0f) {
			return;
		}
		command.transform = CreateTransformMatrix3x2(*transform);
	}
	
	POMDOG_ASSERT(command.texture);
	POMDOG_ASSERT(command.textureRegion.Width > 0);
	POMDOG_ASSERT(command.textureRegion.Height > 0);
	POMDOG_ASSERT(command.textureRegion.Subrect.Width > 0);
	POMDOG_ASSERT(command.textureRegion.Subrect.Height > 0);
	
	renderer.PushCommand(command);
}
//-----------------------------------------------------------------------
void SpriteRenderable::ZOrder(float zOrderIn)
{
	this->zOrder = zOrderIn;
}
//-----------------------------------------------------------------------
float SpriteRenderable::ZOrder() const
{
	return zOrder;
}
//-----------------------------------------------------------------------
void SpriteRenderable::IsVisible(bool isVisibleIn)
{
	this->isVisible = isVisibleIn;
}
//-----------------------------------------------------------------------
bool SpriteRenderable::IsVisible() const
{
	return isVisible;
}
//-----------------------------------------------------------------------
void SpriteRenderable::Color(Pomdog::Color const& colorIn)
{
	command.color = colorIn;
}
//-----------------------------------------------------------------------
Pomdog::Color SpriteRenderable::Color() const
{
	return command.color;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
