//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_FXAA_FAC6D173_4722_460C_A0AF_29925A56EA23_HPP
#define POMDOG_FXAA_FAC6D173_4722_460C_A0AF_29925A56EA23_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class FXAA {
public:
	explicit FXAA(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	void SetViewport(float width, float height);
	void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);
	
	void Apply(GraphicsContext & graphicsContext);
	
private:
	std::shared_ptr<RenderTarget2D> texture;
	std::shared_ptr<SamplerState> samplerLinear;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_FXAA_FAC6D173_4722_460C_A0AF_29925A56EA23_HPP)
