//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP
#define POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "Renderable.hpp"
#include "../Particle2D/ParticleSystem.hpp"
#include "../Rendering/ParticleBatchCommand.hpp"

namespace Pomdog {

class ParticleRenderable: public Renderable {
public:
	ParticleRenderable();

	void Visit(GameObject & gameObject, RenderQueue & renderQueue, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;
	
	void SetZOrder(float zOrder) override;
	float GetZOrder() const override;
	void SetVisible(bool isVisible) override;
	bool IsVisible() const override;
	
	void Load(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);
	void Update(GameObject & gameObject, GameClock const& clock);

public:
	ParticleSystem particleSystem;
	
private:
	Details::Rendering::ParticleBatchCommand command;
	float zOrder;
	bool isVisible;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP)
