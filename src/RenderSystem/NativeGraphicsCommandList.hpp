// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>

namespace Pomdog::Detail {

class NativeGraphicsCommandList {
public:
    NativeGraphicsCommandList() = default;
    NativeGraphicsCommandList(const NativeGraphicsCommandList&) = delete;
    NativeGraphicsCommandList& operator=(const NativeGraphicsCommandList&) = delete;

    virtual ~NativeGraphicsCommandList() = default;

    virtual void Close() = 0;

    virtual void Reset() = 0;

    virtual std::size_t GetCount() const noexcept = 0;

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

    virtual void SetRenderPass(RenderPass&& renderPass) = 0;

    virtual void SetScissorRect(const Rectangle& scissorRect) = 0;

    virtual void SetBlendFactor(const Vector4& blendFactor) = 0;

    virtual void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) = 0;

    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    virtual void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) = 0;

    virtual void SetConstantBuffer(
        int index,
        const std::shared_ptr<NativeBuffer>& constantBuffer,
        std::size_t offset,
        std::size_t sizeInBytes) = 0;

    virtual void SetSampler(int index, std::shared_ptr<NativeSamplerState>&& sampler) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) = 0;
};

} // namespace Pomdog::Detail
