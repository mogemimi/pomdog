// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/Commands/ParticleBatchCommand.hpp"
#include "Renderable.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class ParticleRenderable : public Renderable {
public:
    ParticleRenderable(const std::shared_ptr<Texture2D>& texture);
    //ParticleRenderable(const std::shared_ptr<Texture2D>& texture, const TextureRegion& textureRegion);

    void Visit(Entity& entity, Renderer& renderer) override;

private:
    Detail::Rendering::ParticleBatchCommand command;
};

} // namespace Pomdog
