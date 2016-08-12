// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class NativeGraphicsCommandList {
public:
    NativeGraphicsCommandList() = default;
    NativeGraphicsCommandList(const NativeGraphicsCommandList&) = delete;
    NativeGraphicsCommandList & operator=(const NativeGraphicsCommandList&) = delete;

    virtual ~NativeGraphicsCommandList() = default;

    virtual void Close() = 0;

    virtual void Reset() = 0;

    virtual std::size_t GetCount() const noexcept = 0;

    virtual void Draw(std::size_t vertexCount) = 0;

    virtual void DrawIndexed(std::size_t indexCount) = 0;

    virtual void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) = 0;

    virtual void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) = 0;

    virtual void SetRenderPass(RenderPass && renderPass) = 0;

    virtual void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) = 0;

    virtual void SetBlendFactor(const Color& blendFactor) = 0;

    virtual void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers) = 0;

    virtual void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers) = 0;

    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    virtual void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) = 0;

    virtual void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer) = 0;

    virtual void SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) = 0;
};

} // namespace Detail
} // namespace Pomdog
