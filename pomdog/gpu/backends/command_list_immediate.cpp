// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/memory/placement_new.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::PlacementDelete;
using pomdog::detail::PlacementNew;

namespace pomdog::gpu::detail {
namespace {

struct DrawCommand final : public GraphicsCommand {
    std::uint32_t vertexCount;
    std::uint32_t startVertexLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.Draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final : public GraphicsCommand {
    std::uint32_t indexCount;
    std::uint32_t startIndexLocation;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.DrawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final : public GraphicsCommand {
    std::uint32_t vertexCountPerInstance;
    std::uint32_t instanceCount;
    std::uint32_t startVertexLocation;
    std::uint32_t startInstanceLocation;

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
    std::uint32_t indexCountPerInstance;
    std::uint32_t instanceCount;
    std::uint32_t startIndexLocation;
    std::uint32_t startInstanceLocation;

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
    std::uint32_t slotIndex;
    std::uint32_t offset;

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
    std::uint32_t slotIndex;
    std::uint32_t offset;
    std::uint32_t sizeInBytes;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        static_assert(std::is_unsigned_v<decltype(slotIndex)>, "slotIndex must be >= 0");
        POMDOG_ASSERT(constantBuffer);
        graphicsContext.SetConstantBuffer(slotIndex, constantBuffer, offset, sizeInBytes);
    }
};

struct SetSamplerStateCommand final : public GraphicsCommand {
    std::shared_ptr<SamplerState> sampler;
    std::uint32_t slotIndex;

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
    std::uint32_t slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetTexture(slotIndex, texture);
    }
};

struct SetTextureRenderTarget2DCommand final : public GraphicsCommand {
    std::shared_ptr<RenderTarget2D> texture;
    std::uint32_t slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetTexture(slotIndex, texture);
    }
};

struct SetTextureIndexOnlyCommand final : public GraphicsCommand {
    std::uint32_t slotIndex;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.SetTexture(slotIndex);
    }
};

static_assert(sizeof(SetTextureIndexOnlyCommand) < sizeof(SetTextureCommand));
static_assert(sizeof(SetTextureIndexOnlyCommand) < sizeof(SetTextureRenderTarget2DCommand));

struct BeginRenderPassCommand final : public GraphicsCommand {
    RenderPass renderPass;

    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.BeginRenderPass(renderPass);
    }
};

struct EndRenderPassCommand final : public GraphicsCommand {
    void Execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.EndRenderPass();
    }
};

} // namespace

GraphicsCommand::~GraphicsCommand() = default;

CommandListImmediate::CommandListImmediate() noexcept = default;

CommandListImmediate::~CommandListImmediate()
{
    for (auto& command : commands_) {
        PlacementDelete(allocator_, command);
    }
    for (auto& command : renderPassCommands_) {
        PlacementDelete(allocator_, command);
    }
}

void CommandListImmediate::Close()
{
    if (needToEndRenderPass_) {
        EndRenderPass();
        needToEndRenderPass_ = false;
    }
}

void CommandListImmediate::Reset()
{
    for (auto& command : commands_) {
        PlacementDelete(allocator_, command);
    }
    for (auto& command : renderPassCommands_) {
        PlacementDelete(allocator_, command);
    }

    commands_.clear();
    renderPassCommands_.clear();
    needToEndRenderPass_ = false;

    // NOTE: 16KB
    allocator_.Reset(1024 * 16);
}

void CommandListImmediate::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    FlushRenderPassCommands();

    POMDOG_ASSERT(vertexCount >= 1);
    auto command = PlacementNew<DrawCommand>(allocator_);
    command->vertexCount = static_cast<std::uint32_t>(vertexCount);
    command->startVertexLocation = static_cast<std::uint32_t>(startVertexLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    FlushRenderPassCommands();

    POMDOG_ASSERT(indexCount >= 1);
    auto command = PlacementNew<DrawIndexedCommand>(allocator_);
    command->indexCount = static_cast<std::uint32_t>(indexCount);
    command->startIndexLocation = static_cast<std::uint32_t>(startIndexLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    FlushRenderPassCommands();

    POMDOG_ASSERT(vertexCountPerInstance >= 1);
    auto command = PlacementNew<DrawInstancedCommand>(allocator_);
    command->vertexCountPerInstance = static_cast<std::uint32_t>(vertexCountPerInstance);
    command->instanceCount = static_cast<std::uint32_t>(instanceCount);
    command->startVertexLocation = static_cast<std::uint32_t>(startVertexLocation);
    command->startInstanceLocation = static_cast<std::uint32_t>(startInstanceLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::DrawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    FlushRenderPassCommands();

    POMDOG_ASSERT(indexCountPerInstance >= 1);
    auto command = PlacementNew<DrawIndexedInstancedCommand>(allocator_);
    command->indexCountPerInstance = static_cast<std::uint32_t>(indexCountPerInstance);
    command->instanceCount = static_cast<std::uint32_t>(instanceCount);
    command->startIndexLocation = static_cast<std::uint32_t>(startIndexLocation);
    command->startInstanceLocation = static_cast<std::uint32_t>(startInstanceLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetRenderPass(RenderPass&& renderPass)
{
    if (needToEndRenderPass_) {
        EndRenderPass();
        needToEndRenderPass_ = false;
    }

    auto command = PlacementNew<BeginRenderPassCommand>(allocator_);
    command->renderPass = std::move(renderPass);

    if (command->renderPass.RenderTargets.empty()) {
        command->renderPass.RenderTargets[0] = {nullptr, std::nullopt};
    }
    commands_.push_back(std::move(command));
    needToEndRenderPass_ = true;

    FlushRenderPassCommands();
}

void CommandListImmediate::EndRenderPass()
{
    auto command = PlacementNew<EndRenderPassCommand>(allocator_);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::SetViewport(const Viewport& viewport)
{
    auto command = PlacementNew<SetViewportCommand>(allocator_);
    command->viewport = viewport;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetScissorRect(const Rectangle& scissorRect)
{
    auto command = PlacementNew<SetScissorRectCommand>(allocator_);
    command->scissorRect = scissorRect;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetBlendFactor(const Vector4& blendFactor)
{
    auto command = PlacementNew<SetBlendFactorCommand>(allocator_);
    command->blendFactor = blendFactor;
    renderPassCommands_.push_back(std::move(command));
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
    auto command = PlacementNew<SetVertexBufferCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->vertexBuffer = vertexBuffer;
    command->offset = static_cast<std::uint32_t>(offset);
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer != nullptr);
    auto command = PlacementNew<SetIndexBufferCommand>(allocator_);
    command->indexBuffer = indexBuffer;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = PlacementNew<SetPipelineStateCommand>(allocator_);
    command->pipelineState = pipelineState;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    SetConstantBuffer(index, constantBuffer, 0);
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

    auto command = PlacementNew<SetConstantBufferCommand>(allocator_);
    command->constantBuffer = nativeBuffer;
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->offset = static_cast<std::uint32_t>(offset);
    command->sizeInBytes = static_cast<std::uint32_t>(constantBuffer->GetSizeInBytes() - offset);
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(samplerState != nullptr);
    auto command = PlacementNew<SetSamplerStateCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->sampler = samplerState;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    auto command = PlacementNew<SetTextureIndexOnlyCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture != nullptr);
    auto command = PlacementNew<SetTextureCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->texture = texture;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture != nullptr);
    auto command = PlacementNew<SetTextureRenderTarget2DCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->texture = texture;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::ExecuteImmediate(GraphicsContext& graphicsContext)
{
    for (auto& command : commands_) {
        POMDOG_ASSERT(command != nullptr);
        command->Execute(graphicsContext);
    }
}

void CommandListImmediate::FlushRenderPassCommands()
{
    if (renderPassCommands_.empty()) {
        return;
    }

    commands_.reserve(commands_.size() + renderPassCommands_.size());

    for (auto& command : renderPassCommands_) {
        commands_.push_back(std::move(command));
    }

    renderPassCommands_.clear();
}

} // namespace pomdog::gpu::detail
