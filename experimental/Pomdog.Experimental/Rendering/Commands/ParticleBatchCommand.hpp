// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PARTICLEBATCHCOMMAND_BBF33260_HPP
#define POMDOG_PARTICLEBATCHCOMMAND_BBF33260_HPP

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include "Pomdog.Experimental/Particle2D/Particle.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteRenderer;

namespace Detail {
namespace Rendering {

class ParticleBatchCommand final: public RenderCommand {
public:
    float DrawOrder() const override
    {
        return drawOrder;
    }

    std::type_index TypeIndex() const override;

public:
    TextureRegion textureRegion;
    Matrix3x2 transform;
    std::shared_ptr<Texture2D> texture;
    std::vector<Particle> const* particles = nullptr;
    float drawOrder;
};

} // namespace Rendering
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PARTICLEBATCHCOMMAND_BBF33260_HPP
