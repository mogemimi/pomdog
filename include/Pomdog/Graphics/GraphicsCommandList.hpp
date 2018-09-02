// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT GraphicsCommandList final {
public:
    GraphicsCommandList() = delete;
    GraphicsCommandList(const GraphicsCommandList&) = delete;
    GraphicsCommandList& operator=(const GraphicsCommandList&) = delete;

    explicit GraphicsCommandList(GraphicsDevice& graphicsDevice);

    explicit GraphicsCommandList(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    ~GraphicsCommandList();

    void Close();

    void Reset();

    std::size_t GetCount() const noexcept;

    ///@param vertexCount Number of vertices to draw.
    ///@param startVertexLocation Index of the first vertex to draw.
    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation);

    void DrawIndexed(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount,
        std::size_t startIndexLocation);

    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation);

    void DrawIndexedInstanced(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation);

    void SetRenderPass(RenderPass&& renderPass);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(const Color& blendFactor);

    void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

    void SetVertexBuffer(
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset);

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers);

    void SetVertexBuffers(std::vector<VertexBufferBinding>&& vertexBuffers);

    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState);

    void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer);

    void SetTexture(int index);

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture);

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture);

    void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState);

    Detail::NativeGraphicsCommandList* GetNativeGraphicsCommandList();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandList> nativeCommandList;
};

} // namespace Pomdog
