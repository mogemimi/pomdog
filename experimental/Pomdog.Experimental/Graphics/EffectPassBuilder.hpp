//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASSBUILDER_BFDE27AA_3A8F_4DF7_BAD6_71FBB446BFC6_HPP
#define POMDOG_EFFECTPASSBUILDER_BFDE27AA_3A8F_4DF7_BAD6_71FBB446BFC6_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include <string>
#include <memory>
#include <vector>
#include <cstddef>

namespace Pomdog {

class EffectPassBuilder final {
public:
	explicit EffectPassBuilder(GraphicsDevice & graphicsDevice);

	explicit EffectPassBuilder(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	EffectPassBuilder(EffectPassBuilder &&);
	EffectPassBuilder & operator=(EffectPassBuilder &&);

	~EffectPassBuilder();

	EffectPassBuilder & VertexShaderGLSL(void const* shaderSource, std::size_t byteLength);

	EffectPassBuilder & PixelShaderGLSL(void const* shaderSource, std::size_t byteLength);

	EffectPassBuilder & VertexShaderHLSL(void const* shaderSource, std::size_t byteLength, std::string const& entryPoint);

	EffectPassBuilder & PixelShaderHLSL(void const* shaderSource, std::size_t byteLength, std::string const& entryPoint);

	EffectPassBuilder & InputElements(std::vector<VertexBufferBinding> const& inputElements);

	EffectPassBuilder & InputElements(std::vector<VertexBufferBinding> && inputElements);

	EffectPassBuilder & InputElements(VertexDeclaration const& vertexDeclaration);

	EffectPassBuilder & InputElements(VertexDeclaration && vertexDeclaration);

	std::shared_ptr<EffectPass> Create();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSBUILDER_BFDE27AA_3A8F_4DF7_BAD6_71FBB446BFC6_HPP)
