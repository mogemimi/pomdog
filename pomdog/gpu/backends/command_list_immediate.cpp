// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/memory/placement_new.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
namespace {

struct DrawCommand final : public GraphicsCommand {
    u32 vertexCount;
    u32 startVertexLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final : public GraphicsCommand {
    u32 indexCount;
    u32 startIndexLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.drawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final : public GraphicsCommand {
    u32 vertexCountPerInstance;
    u32 instanceCount;
    u32 startVertexLocation;
    u32 startInstanceLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.drawInstanced(
            vertexCountPerInstance,
            instanceCount,
            startVertexLocation,
            startInstanceLocation);
    }
};

struct DrawIndexedInstancedCommand final : public GraphicsCommand {
    u32 indexCountPerInstance;
    u32 instanceCount;
    u32 startIndexLocation;
    u32 startInstanceLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.drawIndexedInstanced(
            indexCountPerInstance,
            instanceCount,
            startIndexLocation,
            startInstanceLocation);
    }
};

struct SetViewportCommand final : public GraphicsCommand {
    Viewport viewport;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setViewport(viewport);
    }
};

struct SetScissorRectCommand final : public GraphicsCommand {
    Rect2D scissorRect;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setScissorRect(scissorRect);
    }
};

struct SetBlendFactorCommand final : public GraphicsCommand {
    Vector4 blendFactor;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setBlendFactor(blendFactor);
    }
};

struct SetVertexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<VertexBuffer> vertexBuffer;
    u32 slotIndex;
    u32 offset;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setVertexBuffer(slotIndex, vertexBuffer, offset);
    }
};

struct SetIndexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<IndexBuffer> indexBuffer;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setIndexBuffer(indexBuffer);
    }
};

struct SetPipelineStateCommand final : public GraphicsCommand {
    std::shared_ptr<PipelineState> pipelineState;

    void execute(GraphicsContext& graphicsContext) const override
    {
        POMDOG_ASSERT(pipelineState);
        graphicsContext.setPipelineState(pipelineState);
    }
};

struct SetConstantBufferCommand final : public GraphicsCommand {
    std::shared_ptr<Buffer> constantBuffer;
    u32 slotIndex;
    u32 offset;
    u32 sizeInBytes;

    void execute(GraphicsContext& graphicsContext) const override
    {
        static_assert(std::is_unsigned_v<decltype(slotIndex)>, "slotIndex must be >= 0");
        POMDOG_ASSERT(constantBuffer);
        graphicsContext.setConstantBuffer(slotIndex, constantBuffer, offset, sizeInBytes);
    }
};

struct SetSamplerStateCommand final : public GraphicsCommand {
    std::shared_ptr<SamplerState> sampler;
    u32 slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        if (sampler) {
            graphicsContext.setSampler(slotIndex, sampler);
        }
        else {
            graphicsContext.setSampler(slotIndex, sampler);
        }
    }
};

struct SetTextureCommand final : public GraphicsCommand {
    std::shared_ptr<gpu::Texture2D> texture;
    u32 slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setTexture(slotIndex, texture);
    }
};

struct SetTextureRenderTarget2DCommand final : public GraphicsCommand {
    std::shared_ptr<RenderTarget2D> texture;
    u32 slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setTexture(slotIndex, texture);
    }
};

struct SetTextureIndexOnlyCommand final : public GraphicsCommand {
    u32 slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setTexture(slotIndex);
    }
};

static_assert(sizeof(SetTextureIndexOnlyCommand) < sizeof(SetTextureCommand));
static_assert(sizeof(SetTextureIndexOnlyCommand) < sizeof(SetTextureRenderTarget2DCommand));

struct BeginRenderPassCommand final : public GraphicsCommand {
    RenderPass renderPass;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.beginRenderPass(renderPass);
    }
};

struct EndRenderPassCommand final : public GraphicsCommand {
    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.endRenderPass();
    }
};

} // namespace

GraphicsCommand::~GraphicsCommand() = default;

CommandListImmediate::CommandListImmediate() noexcept = default;

CommandListImmediate::~CommandListImmediate()
{
    for (auto& command : commands_) {
        memory::placementDelete(allocator_, command);
    }
    for (auto& command : renderPassCommands_) {
        memory::placementDelete(allocator_, command);
    }
}

void CommandListImmediate::close()
{
    // NOTE: This is a safety net for the case where the user forgets to call
    //       endRenderPass() before close(). Ideally, the user should always
    //       explicitly call endRenderPass() to end the render pass scope.
    // TODO: In debug builds, emit a warning log when this safety net is
    //       triggered, or consider adding a command list validation API to detect
    //       mismatched beginRenderPass/endRenderPass calls.
    if (needToEndRenderPass_) {
        endRenderPass();
        needToEndRenderPass_ = false;
    }
}

void CommandListImmediate::reset()
{
    for (auto& command : commands_) {
        memory::placementDelete(allocator_, command);
    }
    for (auto& command : renderPassCommands_) {
        memory::placementDelete(allocator_, command);
    }

    commands_.clear();
    renderPassCommands_.clear();
    needToEndRenderPass_ = false;

    // NOTE: 16KB
    allocator_.reset(1024 * 16);
}

void CommandListImmediate::draw(
    u32 vertexCount,
    u32 startVertexLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(vertexCount >= 1);
    auto command = memory::placementNew<DrawCommand>(allocator_);
    command->vertexCount = vertexCount;
    command->startVertexLocation = startVertexLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawIndexed(
    u32 indexCount,
    u32 startIndexLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(indexCount >= 1);
    auto command = memory::placementNew<DrawIndexedCommand>(allocator_);
    command->indexCount = indexCount;
    command->startIndexLocation = startIndexLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawInstanced(
    u32 vertexCountPerInstance,
    u32 instanceCount,
    u32 startVertexLocation,
    u32 startInstanceLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(vertexCountPerInstance >= 1);
    auto command = memory::placementNew<DrawInstancedCommand>(allocator_);
    command->vertexCountPerInstance = vertexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startVertexLocation = startVertexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawIndexedInstanced(
    u32 indexCountPerInstance,
    u32 instanceCount,
    u32 startIndexLocation,
    u32 startInstanceLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(indexCountPerInstance >= 1);
    auto command = memory::placementNew<DrawIndexedInstancedCommand>(allocator_);
    command->indexCountPerInstance = indexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startIndexLocation = startIndexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands_.push_back(std::move(command));
}

void CommandListImmediate::beginRenderPass(RenderPass&& renderPass)
{
    if (needToEndRenderPass_) {
        endRenderPass();
    }

    auto command = memory::placementNew<BeginRenderPassCommand>(allocator_);
    command->renderPass = std::move(renderPass);

    if (command->renderPass.renderTargets.empty()) {
        command->renderPass.renderTargets[0] = {nullptr, std::nullopt};
    }
    commands_.push_back(std::move(command));
    needToEndRenderPass_ = true;

    flushRenderPassCommands();
}

void CommandListImmediate::endRenderPass()
{
    flushRenderPassCommands();

    auto command = memory::placementNew<EndRenderPassCommand>(allocator_);
    commands_.push_back(std::move(command));
    needToEndRenderPass_ = false;
}

void CommandListImmediate::setViewport(const Viewport& viewport)
{
    auto command = memory::placementNew<SetViewportCommand>(allocator_);
    command->viewport = viewport;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setScissorRect(const Rect2D& scissorRect)
{
    auto command = memory::placementNew<SetScissorRectCommand>(allocator_);
    command->scissorRect = scissorRect;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setBlendFactor(const Vector4& blendFactor)
{
    auto command = memory::placementNew<SetBlendFactorCommand>(allocator_);
    command->blendFactor = blendFactor;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setVertexBuffer(
    u32 index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    setVertexBuffer(index, vertexBuffer, 0);
}

void CommandListImmediate::setVertexBuffer(
    u32 index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    u32 offset)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");
    POMDOG_ASSERT(vertexBuffer != nullptr);
    auto command = memory::placementNew<SetVertexBufferCommand>(allocator_);
    command->slotIndex = index;
    command->vertexBuffer = vertexBuffer;
    command->offset = offset;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer != nullptr);
    auto command = memory::placementNew<SetIndexBufferCommand>(allocator_);
    command->indexBuffer = indexBuffer;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = memory::placementNew<SetPipelineStateCommand>(allocator_);
    command->pipelineState = pipelineState;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setConstantBuffer(
    u32 index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    setConstantBuffer(index, constantBuffer, 0);
}

void CommandListImmediate::setConstantBuffer(
    u32 index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
    u32 offset)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");
    POMDOG_ASSERT(constantBuffer);

    std::shared_ptr<Buffer> nativeBuffer{constantBuffer, constantBuffer->getBuffer()};

    auto command = memory::placementNew<SetConstantBufferCommand>(allocator_);
    command->constantBuffer = nativeBuffer;
    command->slotIndex = index;
    command->offset = offset;
    command->sizeInBytes = constantBuffer->getSizeInBytes() - offset;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setSamplerState(u32 index, const std::shared_ptr<SamplerState>& samplerState)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    POMDOG_ASSERT(samplerState != nullptr);
    auto command = memory::placementNew<SetSamplerStateCommand>(allocator_);
    command->slotIndex = index;
    command->sampler = samplerState;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(u32 index)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    auto command = memory::placementNew<SetTextureIndexOnlyCommand>(allocator_);
    command->slotIndex = index;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(u32 index, const std::shared_ptr<gpu::Texture2D>& texture)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    POMDOG_ASSERT(texture != nullptr);
    auto command = memory::placementNew<SetTextureCommand>(allocator_);
    command->slotIndex = index;
    command->texture = texture;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(u32 index, const std::shared_ptr<RenderTarget2D>& texture)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index >= 0");
    POMDOG_ASSERT(texture != nullptr);
    auto command = memory::placementNew<SetTextureRenderTarget2DCommand>(allocator_);
    command->slotIndex = index;
    command->texture = texture;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::executeImmediate(GraphicsContext& graphicsContext)
{
    for (auto& command : commands_) {
        POMDOG_ASSERT(command != nullptr);
        command->execute(graphicsContext);
    }
}

void CommandListImmediate::flushRenderPassCommands()
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
