// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
namespace {

struct DrawCommand final : public GraphicsCommand {
    std::size_t vertexCount;
    std::size_t startVertexLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.Draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final : public GraphicsCommand {
    std::size_t indexCount;
    std::size_t startIndexLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.DrawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final : public GraphicsCommand {
    std::size_t vertexCountPerInstance;
    std::size_t instanceCount;
    std::size_t startVertexLocation;
    std::size_t startInstanceLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.DrawInstanced(
            vertexCountPerInstance,
            instanceCount,
            startVertexLocation,
            startInstanceLocation);
    }
};

struct DrawIndexedInstancedCommand final : public GraphicsCommand {
    std::size_t indexCountPerInstance;
    std::size_t instanceCount;
    std::size_t startIndexLocation;
    std::size_t startInstanceLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.DrawIndexedInstanced(
            indexCountPerInstance,
            instanceCount,
            startIndexLocation,
            startInstanceLocation);
    }
};

struct SetViewportCommand final : public GraphicsCommand {
    Viewport viewport;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetViewport(viewport);
    }
};

struct SetScissorRectCommand final : public GraphicsCommand {
    Rectangle scissorRect;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetScissorRect(scissorRect);
    }
};

struct SetBlendFactorCommand final : public GraphicsCommand {
    Vector4 blendFactor;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetBlendFactor(blendFactor);
    }
};

struct SetVertexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<VertexBuffer> vertexBuffer;
    int slotIndex;
    std::size_t offset;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetVertexBuffer(slotIndex, vertexBuffer, offset);
    }
};

struct SetIndexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<IndexBuffer> indexBuffer;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetIndexBuffer(indexBuffer);
    }
};

struct SetPipelineStateCommand final : public GraphicsCommand {
    std::shared_ptr<PipelineState> pipelineState;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        POMDOG_ASSERT(pipelineState);
        graphicsContext.SetPipelineState(pipelineState);
    }
};

struct SetConstantBufferCommand final : public GraphicsCommand {
    std::shared_ptr<Buffer> constantBuffer;
    int slotIndex;
    std::size_t offset;
    std::size_t sizeInBytes;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        POMDOG_ASSERT(constantBuffer);
        POMDOG_ASSERT(slotIndex >= 0);
        graphicsContext.SetConstantBuffer(slotIndex, constantBuffer, offset, sizeInBytes);
    }
};

struct SetSamplerStateCommand final : public GraphicsCommand {
    std::shared_ptr<SamplerState> sampler;
    int slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        if (sampler) {
            graphicsContext.SetSampler(slotIndex, sampler);
        }
        else {
            graphicsContext.SetSampler(slotIndex, sampler);
        }
    }
};

struct SetTextureCommand final : public GraphicsCommand {
    std::shared_ptr<gpu::Texture2D> texture;
    int slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        if (texture) {
            graphicsContext.SetTexture(slotIndex, texture);
        }
        else {
            graphicsContext.SetTexture(slotIndex);
        }
    }
};

struct SetTextureRenderTarget2DCommand final : public GraphicsCommand {
    std::shared_ptr<RenderTarget2D> texture;
    int slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        if (texture) {
            graphicsContext.SetTexture(slotIndex, texture);
        }
        else {
            graphicsContext.SetTexture(slotIndex);
        }
    }
};

struct SetRenderPassCommand final : public GraphicsCommand {
    RenderPass renderPass;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetRenderPass(renderPass);
    }
};

} // namespace

GraphicsCommand::~GraphicsCommand() = default;

CommandListImmediate::CommandListImmediate() noexcept = default;

CommandListImmediate::~CommandListImmediate() = default;

void CommandListImmediate::Close()
{
}

void CommandListImmediate::Reset()
{
    commands_.clear();
}

std::size_t CommandListImmediate::GetCount() const noexcept
{
    return commands_.size();
}

void CommandListImmediate::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<DrawCommand>();
    command->vertexCount = vertexCount;
    command->startVertexLocation = startVertexLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<DrawIndexedCommand>();
    command->indexCount = indexCount;
    command->startIndexLocation = startIndexLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(vertexCountPerInstance >= 1);
    auto command = std::make_unique<DrawInstancedCommand>();
    command->vertexCountPerInstance = vertexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startVertexLocation = startVertexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(indexCountPerInstance >= 1);
    auto command = std::make_unique<DrawIndexedInstancedCommand>();
    command->indexCountPerInstance = indexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startIndexLocation = startIndexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetRenderPass(RenderPass&& renderPass)
{
    auto command = std::make_unique<SetRenderPassCommand>();
    command->renderPass = std::move(renderPass);

    if (command->renderPass.RenderTargets.empty()) {
        command->renderPass.RenderTargets[0] = {nullptr, std::nullopt};
    }
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetViewport(const Viewport& viewport)
{
    auto command = std::make_unique<SetViewportCommand>();
    command->viewport = viewport;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetScissorRect(const Rectangle& scissorRect)
{
    auto command = std::make_unique<SetScissorRectCommand>();
    command->scissorRect = scissorRect;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetBlendFactor(const Vector4& blendFactor)
{
    auto command = std::make_unique<SetBlendFactorCommand>();
    command->blendFactor = blendFactor;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    SetVertexBuffer(index, vertexBuffer, 0);
}

void CommandListImmediate::SetVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(vertexBuffer != nullptr);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");
    auto command = std::make_unique<SetVertexBufferCommand>();
    command->slotIndex = index;
    command->vertexBuffer = vertexBuffer;
    command->offset = offset;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer != nullptr);
    auto command = std::make_unique<SetIndexBufferCommand>();
    command->indexBuffer = indexBuffer;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = std::make_unique<SetPipelineStateCommand>();
    command->pipelineState = pipelineState;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);

    std::shared_ptr<Buffer> nativeBuffer{constantBuffer, constantBuffer->GetBuffer()};

    auto command = std::make_unique<SetConstantBufferCommand>();
    command->constantBuffer = nativeBuffer;
    command->slotIndex = index;
    command->offset = 0;
    command->sizeInBytes = constantBuffer->GetSizeInBytes();
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");

    std::shared_ptr<Buffer> nativeBuffer{constantBuffer, constantBuffer->GetBuffer()};

    auto command = std::make_unique<SetConstantBufferCommand>();
    command->constantBuffer = nativeBuffer;
    command->slotIndex = index;
    command->offset = offset;
    command->sizeInBytes = constantBuffer->GetSizeInBytes() - offset;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(samplerState != nullptr);
    auto command = std::make_unique<SetSamplerStateCommand>();
    command->slotIndex = index;
    command->sampler = samplerState;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    auto command = std::make_unique<SetTextureCommand>();
    command->slotIndex = index;
    command->texture = nullptr;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureRenderTarget2DCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::ExecuteImmediate(GraphicsContext& graphicsContext)
{
    for (auto& command : commands_) {
        POMDOG_ASSERT(command != nullptr);
        command->Execute(graphicsContext);
    }
}

} // namespace pomdog::gpu::detail
