// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP
#define POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

}// namespace Rendering
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP)
