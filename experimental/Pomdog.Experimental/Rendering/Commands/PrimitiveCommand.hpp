// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PRIMITIVECOMMAND_4CCAFA9C_HPP
#define POMDOG_PRIMITIVECOMMAND_4CCAFA9C_HPP

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Rendering {

class PrimitiveCommand final: public RenderCommand {
public:
    float DrawOrder() const override
    {
        return drawOrder;
    }

    std::type_index TypeIndex() const override;

public:
    Matrix3x2 transform;
    //Vector2 originPivot;
    Rectangle rectangle;
    Color leftTopColor;
    Color rightTopColor;
    Color leftBottomColor;
    Color rightBottomColor;
    float drawOrder;
};

}// namespace Rendering
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_PRIMITIVECOMMAND_4CCAFA9C_HPP
