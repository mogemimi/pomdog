//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VIGNETTEEFFECT_395F49A8_9033_426B_AD02_FD8563F40232_HPP
#define POMDOG_VIGNETTEEFFECT_395F49A8_9033_426B_AD02_FD8563F40232_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class VignetteEffect {
public:
	explicit VignetteEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	void SetViewport(float width, float height);
	
	void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);
	
	void SetIntensity(float intensity);
	
	void Apply(GraphicsContext & graphicsContext);
	
private:
	std::shared_ptr<RenderTarget2D> texture;
	std::shared_ptr<SamplerState> samplerLinear;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_VIGNETTEEFFECT_395F49A8_9033_426B_AD02_FD8563F40232_HPP)
