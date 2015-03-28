// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSBUILDER_BFDE27AA_HPP
#define POMDOG_EFFECTPASSBUILDER_BFDE27AA_HPP

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

    EffectPassBuilder & VertexShaderHLSLPrecompiled(void const* shaderSource, std::size_t byteLength);

    EffectPassBuilder & PixelShaderHLSLPrecompiled(void const* shaderSource, std::size_t byteLength);

    EffectPassBuilder & InputElements(std::vector<VertexBufferBinding> const& inputElements);

    EffectPassBuilder & InputElements(std::vector<VertexBufferBinding> && inputElements);

    EffectPassBuilder & InputElements(VertexDeclaration const& vertexDeclaration);

    EffectPassBuilder & InputElements(VertexDeclaration && vertexDeclaration);

    EffectPassBuilder & BlendState(BlendDescription const& blendState);

    EffectPassBuilder & RasterizerState(RasterizerDescription const& rasterizerState);

    EffectPassBuilder & DepthStencilState(DepthStencilDescription const& depthStencilState);

    std::shared_ptr<EffectPass> Create();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // POMDOG_EFFECTPASSBUILDER_BFDE27AA_HPP
