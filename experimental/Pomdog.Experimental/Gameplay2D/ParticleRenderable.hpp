// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/ParticleBatchCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class ParticleRenderable: public Renderable {
public:
    ParticleRenderable(std::shared_ptr<Texture2D> const& texture);
    //ParticleRenderable(std::shared_ptr<Texture2D> const& texture, TextureRegion const& textureRegion);

    void Visit(Entity & entity, Renderer & renderer) override;

private:
    Detail::Rendering::ParticleBatchCommand command;
};

} // namespace Pomdog
