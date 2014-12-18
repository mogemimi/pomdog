//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP
#define POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/ParticleBatchCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class ParticleRenderable: public Renderable {
public:
	ParticleRenderable(std::shared_ptr<Texture2D> const& texture, std::shared_ptr<BlendState> const& blendState);
	//ParticleRenderable(std::shared_ptr<Texture2D> const& texture, TextureRegion const& textureRegion);

	void Visit(GameObject & gameObject, Renderer & renderer) override;
	
private:
	Details::Rendering::ParticleBatchCommand command;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLERENDERABLE_1729BB9F_0595_424B_988E_D38957B3BA8B_HPP)
