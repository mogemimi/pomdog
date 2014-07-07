//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BEAMRENDERER_C48BC873_9ED7_4C3C_B244_4A839E4E2407_HPP
#define POMDOG_BEAMRENDERER_C48BC873_9ED7_4C3C_B244_4A839E4E2407_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "Renderer.hpp"
#include "../Particle2D/BeamSystem.hpp"

namespace Pomdog {

class BeamRenderer: public Renderer {
public:
	BeamRenderer();

	void Visit(GameObject & gameObject, RenderQueue & renderQueue, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;
	
	void SetZOrder(float zOrder) override;
	float GetZOrder() const override;
	void SetVisible(bool isVisible) override;
	bool IsVisible() const override;
	
	void Load(std::shared_ptr<GraphicsDevice> const& graphicsDevice, std::shared_ptr<AssetManager> const& assets);
	void Update(GameClock const& clock);
	
	void DrawBeam();
	
private:
	BeamSystem beamSystem;
	std::shared_ptr<Texture2D> beamTexture;
	
	float zOrder;
	bool isVisible;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAMRENDERER_C48BC873_9ED7_4C3C_B244_4A839E4E2407_HPP)
