// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FISHEYEEFFECT_62F772C0_3432_453B_A70D_D857D9CAEFB7_HPP
#define POMDOG_FISHEYEEFFECT_62F772C0_3432_453B_A70D_D857D9CAEFB7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class FishEyeEffect {
public:
	explicit FishEyeEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void SetViewport(float width, float height);
	void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);

	void Apply(GraphicsContext & graphicsContext);

private:
	std::shared_ptr<RenderTarget2D> texture;
	std::shared_ptr<SamplerState> samplerLinear;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

}// namespace Pomdog

#endif // !defined(POMDOG_FISHEYEEFFECT_62F772C0_3432_453B_A70D_D857D9CAEFB7_HPP)
