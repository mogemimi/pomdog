// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class GraphicsCommandListImmediate;
struct GraphicsCapabilities;

class NativeGraphicsContext {
public:
    NativeGraphicsContext() = default;
    NativeGraphicsContext(const NativeGraphicsContext&) = delete;
    NativeGraphicsContext & operator=(const NativeGraphicsContext&) = delete;

    virtual ~NativeGraphicsContext() = default;

    ///@brief Retrieves the capabilities of a GraphicsContext
    virtual GraphicsCapabilities GetCapabilities() const = 0;

    virtual void ExecuteCommandLists(
        const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists) = 0;

    virtual void Present() = 0;

    virtual void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) = 0;

    virtual void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) = 0;

    virtual void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) = 0;

    virtual void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) = 0;

    virtual void SetRenderPass(const RenderPass& renderPass) = 0;

    virtual void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) = 0;

    virtual void SetBlendFactor(const Color& blendFactor) = 0;

    virtual void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers) = 0;

    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    virtual void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) = 0;

    virtual void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer) = 0;

    virtual void SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) = 0;
};

} // namespace Detail
} // namespace Pomdog
