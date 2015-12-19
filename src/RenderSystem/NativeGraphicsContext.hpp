// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <vector>
#include <memory>

namespace Pomdog {
namespace Detail {

struct GraphicsCapabilities;

class NativeGraphicsContext : Noncopyable {
public:
    virtual ~NativeGraphicsContext() = default;

    ///@brief Retrieves the capabilities of a GraphicsContext
    virtual GraphicsCapabilities GetCapabilities() const = 0;

    virtual void Present() = 0;

    virtual void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil) = 0;

    virtual void Draw(std::size_t vertexCount) = 0;

    virtual void DrawIndexed(std::size_t indexCount) = 0;

    virtual void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) = 0;

    virtual void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) = 0;

    virtual void SetViewport(Viewport const& viewport) = 0;

    virtual void SetScissorRectangle(Rectangle const& rectangle) = 0;

    virtual void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) = 0;

    virtual void SetBlendFactor(Color const& blendFactor) = 0;

    virtual void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers) = 0;

    virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer) = 0;

    virtual void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) = 0;

    virtual void SetConstantBuffer(int index, std::shared_ptr<NativeBuffer> const& constantBuffer) = 0;

    virtual void SetSampler(int index, NativeSamplerState* sampler) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, Texture2D & texture) = 0;

    virtual void SetTexture(int index, RenderTarget2D & texture) = 0;

    virtual void SetRenderTarget() = 0;

    virtual void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) = 0;
};

} // namespace Detail
} // namespace Pomdog
