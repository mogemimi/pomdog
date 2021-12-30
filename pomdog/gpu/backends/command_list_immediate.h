// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

class NativeGraphicsContext;

enum class GraphicsCommandType : std::int8_t {
    DrawCommand,
    DrawIndexedCommand,
    DrawInstancedCommand,
    DrawIndexedInstancedCommand,
    SetRenderPassCommand,
    SetViewportCommand,
    SetScissorRectCommand,
    SetBlendFactorCommand,
    SetVertexBufferCommand,
    SetIndexBufferCommand,
    SetPipelineStateCommand,
    SetConstantBufferCommand,
    SetSamplerStateCommand,
    SetTextureCommand,
    SetTextureRenderTarget2DCommand,
};

class GraphicsCommand {
public:
    virtual ~GraphicsCommand();

    virtual void Execute(NativeGraphicsContext& graphicsContext) = 0;

    GraphicsCommandType commandType;
};

class GraphicsCommandListImmediate final : public GraphicsCommandList {
public:
    GraphicsCommandListImmediate() = default;
    GraphicsCommandListImmediate(const GraphicsCommandListImmediate&) = delete;
    GraphicsCommandListImmediate& operator=(const GraphicsCommandListImmediate&) = delete;

    ~GraphicsCommandListImmediate() override;

    /// Declares that recording to the command list is completed.
    void Close() override;

    /// Clears the graphics commands.
    void Reset() override;

    /// Gets the count of graphics commands.
    std::size_t GetCount() const noexcept override;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    /// Draws the specified indexed primitives.
    void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    /// Draws the specified instanced primitives.
    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    /// Draws the specified indexed, instanced primitives.
    void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    /// Sets a group of render targets.
    void SetRenderPass(RenderPass&& renderPass) override;

    /// Sets the viewport dynamically to the rasterizer stage.
    void SetViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle dynamically for a scissor test.
    void SetScissorRect(const Rectangle& scissorRect) override;

    /// Sets the constant blend color and alpha values.
    void SetBlendFactor(const Vector4& blendFactor) override;

    /// Sets a vertex buffer.
    void SetVertexBuffer(int index, const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

    /// Sets a vertex buffer.
    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    /// Sets an index buffer.
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    /// Sets a pipeline state.
    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Sets a constant buffer.
    void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

    /// Sets a constant buffer.
    void SetConstantBuffer(
        int index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t offset) override;

    /// Sets an empty texture to the specified slot.
    void SetTexture(int index) override;

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    /// Sets a sampler state to the specified slot.
    void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState) override;

    void ExecuteImmediate(NativeGraphicsContext& graphicsContext);

private:
    void SortCommandsForMetal();

private:
    std::vector<std::shared_ptr<detail::GraphicsCommand>> commands;
};

} // namespace pomdog::gpu::detail
