// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Particle2D/BeamSystem.hpp"
#include "Renderable.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include <memory>

namespace Pomdog {

class BeamRenderable final : public Renderable {
public:
    BeamRenderable();

    void Visit(Entity& entity, Renderer& renderer) override;

    void Load(const std::shared_ptr<GraphicsDevice>& graphicsDevice, const std::shared_ptr<AssetManager>& assets);
    void Update(const GameClock& clock);

    void DrawBeam();

private:
    BeamSystem beamSystem;
    std::shared_ptr<Texture2D> texture;
};

} // namespace Pomdog
