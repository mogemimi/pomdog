// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_BEAMRENDERABLE_5F289224_HPP
#define POMDOG_BEAMRENDERABLE_5F289224_HPP

#include "Renderable.hpp"
#include "Pomdog.Experimental/Particle2D/BeamSystem.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include <memory>

namespace Pomdog {

class BeamRenderable: public Renderable {
public:
    BeamRenderable();

    void Visit(GameObject & gameObject, Renderer & renderer) override;

    void Load(std::shared_ptr<GraphicsDevice> const& graphicsDevice, std::shared_ptr<AssetManager> const& assets);
    void Update(GameClock const& clock);

    void DrawBeam();

private:
    BeamSystem beamSystem;
    std::shared_ptr<Texture2D> texture;
};

}// namespace Pomdog

#endif // POMDOG_BEAMRENDERABLE_5F289224_HPP
