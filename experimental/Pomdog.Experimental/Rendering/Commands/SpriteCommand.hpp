// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Rendering {

class SpriteCommand final: public RenderCommand {
public:
    float DrawOrder() const override
    {
        return drawOrder;
    }

    std::type_index TypeIndex() const override;

public:
    Matrix3x2 transform;
    TextureRegion textureRegion;
    std::shared_ptr<Texture2D> texture;
    Vector2 originPivot;
    Color color;
    float drawOrder;
};

} // namespace Rendering
} // namespace Detail
} // namespace Pomdog
