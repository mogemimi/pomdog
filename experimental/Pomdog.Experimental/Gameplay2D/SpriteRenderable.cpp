// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteRenderable.hpp"
#include "../Rendering/Renderer.hpp"
#include <Pomdog.Experimental/Gameplay2D/Transform2D.hpp>
#include <limits>

namespace Pomdog {
namespace {

static Matrix3x2 CreateTransformMatrix3x2(Transform2D const& transform)
{
    return Matrix3x2::CreateScale(transform.Scale)
        * Matrix3x2::CreateRotation(transform.Rotation)
        * Matrix3x2::CreateTranslation(transform.Position);
}

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
    return std::move(textureRegion);
}

namespace SpriteRenderableDirtyFlags {

static constexpr std::uint32_t OriginPivot = 0x1;
static constexpr std::uint32_t Region = 0x2;

}// namespace SpriteRenderableDirtyFlags

}// unnamed namespace

SpriteRenderable::SpriteRenderable(std::shared_ptr<Texture2D> const& texture)
    : SpriteRenderable(texture, CreateTextureRegionFromTexture(texture))
{}

SpriteRenderable::SpriteRenderable(std::shared_ptr<Texture2D> const& textureIn, TextureRegion const& textureRegionIn)
    : originPivot{0.5f, 0.5f}
    , dirtyFlags{std::numeric_limits<std::uint32_t>::max()}
{
    POMDOG_ASSERT(textureIn);
    command.texture = textureIn;
    command.textureRegion = textureRegionIn;
    command.transform = Matrix3x2::Identity;
    command.color = Material.Color;
    command.originPivot = {0.5f, 0.5f};
    command.drawOrder = 0;
}

void SpriteRenderable::Visit(GameObject & gameObject, Renderer & renderer)
{
    if (!IsVisible) {
        return;
    }

    if (dirtyFlags & SpriteRenderableDirtyFlags::Region)
    {
        auto & region = command.textureRegion;
        offsetMatrix = Matrix3x2::CreateTranslation(Vector2(region.XOffset, region.Height - (region.YOffset + region.Subrect.Height)));
        command.originPivot = originPivot * Vector2(region.Width, region.Height) / Vector2(region.Subrect.Width, region.Subrect.Height);
    }
    else if (dirtyFlags & SpriteRenderableDirtyFlags::OriginPivot)
    {
        auto & region = command.textureRegion;
        command.originPivot = originPivot * Vector2(region.Width, region.Height) / Vector2(region.Subrect.Width, region.Subrect.Height);
    }
    dirtyFlags = 0;

    command.drawOrder = DrawOrder;
    command.color = Material.Color;

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

    POMDOG_ASSERT(command.texture);
    POMDOG_ASSERT(command.textureRegion.Width > 0);
    POMDOG_ASSERT(command.textureRegion.Height > 0);
    POMDOG_ASSERT(command.textureRegion.Subrect.Width > 0);
    POMDOG_ASSERT(command.textureRegion.Subrect.Height > 0);

    renderer.PushCommand(command);
}

void SpriteRenderable::OriginPivot(Vector2 const& originPivotIn)
{
    originPivot = originPivotIn;
    dirtyFlags |= SpriteRenderableDirtyFlags::OriginPivot;
}

Vector2 SpriteRenderable::OriginPivot() const
{
    return originPivot;
}

void SpriteRenderable::Region(TextureRegion const& regionIn)
{
    command.textureRegion = regionIn;
    dirtyFlags |= SpriteRenderableDirtyFlags::Region;
}

TextureRegion const& SpriteRenderable::Region() const
{
    return command.textureRegion;
}

Rectangle SpriteRenderable::BoundingBox() const
{
    return {0, 0, command.textureRegion.Width, command.textureRegion.Height};
}

}// namespace Pomdog
