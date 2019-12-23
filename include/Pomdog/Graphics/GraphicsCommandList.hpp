// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GraphicsCommandList final {
public:
    GraphicsCommandList() = delete;
    GraphicsCommandList(const GraphicsCommandList&) = delete;
    GraphicsCommandList& operator=(const GraphicsCommandList&) = delete;

    explicit GraphicsCommandList(GraphicsDevice& graphicsDevice);

    explicit GraphicsCommandList(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    ~GraphicsCommandList();

    /// Declares that recording to the command list is completed.
    void Close();

    /// Clears the graphics commands.
    void Reset();

    /// Gets the count of graphics commands.
    std::size_t GetCount() const noexcept;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation);

    /// Draws the specified indexed primitives.
    void DrawIndexed(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount,
        std::size_t startIndexLocation);

    /// Draws the specified instanced primitives.
    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation);

    /// Draws the specified indexed, instanced primitives.
    void DrawIndexedInstanced(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation);

    /// Sets a group of render targets.
    void SetRenderPass(RenderPass&& renderPass);

    /// Specifies the geometric primitive type for the input assembler stage.
    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    /// Sets the scissor rectangle dynamically for a scissor test.
    void SetScissorRect(const Rectangle& scissorRect);

    /// Sets the constant blend color and alpha values.
    void SetBlendFactor(const Vector4& blendFactor);

    /// Sets a vertex buffer.
    void SetVertexBuffer(int index, const std::shared_ptr<VertexBuffer>& vertexBuffer);

    /// Sets a vertex buffer.
    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset);

    /// Sets a pipeline state.
    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState);

    /// Sets a constant buffer.
    void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer);

    /// Sets a constant buffer.
    void SetConstantBuffer(
        int index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t offset);

    /// Sets an empty texture to the specified slot.
    void SetTexture(int index);

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture);

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture);

    /// Sets a sampler state to the specified slot.
    void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState);

    /// Gets the pointer of the native graphics command list.
    Detail::NativeGraphicsCommandList* GetNativeGraphicsCommandList();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandList> nativeCommandList;
};

} // namespace Pomdog
