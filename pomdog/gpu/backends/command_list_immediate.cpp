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

namespace pomdog::gpu::detail {
namespace {

struct DrawCommand final : public GraphicsCommand {
    std::uint32_t vertexCount;
    std::uint32_t startVertexLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final : public GraphicsCommand {
    std::uint32_t indexCount;
    std::uint32_t startIndexLocation;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.drawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final : public GraphicsCommand {
    std::uint32_t vertexCountPerInstance;
    std::uint32_t instanceCount;
    std::uint32_t startVertexLocation;
    std::uint32_t startInstanceLocation;

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
    std::uint32_t indexCountPerInstance;
    std::uint32_t instanceCount;
    std::uint32_t startIndexLocation;
    std::uint32_t startInstanceLocation;

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
    Rectangle scissorRect;

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
    std::uint32_t slotIndex;
    std::uint32_t offset;

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
    std::uint32_t slotIndex;
    std::uint32_t offset;
    std::uint32_t sizeInBytes;

    void execute(GraphicsContext& graphicsContext) const override
    {
        static_assert(std::is_unsigned_v<decltype(slotIndex)>, "slotIndex must be >= 0");
        POMDOG_ASSERT(constantBuffer);
        graphicsContext.setConstantBuffer(slotIndex, constantBuffer, offset, sizeInBytes);
    }
};

struct SetSamplerStateCommand final : public GraphicsCommand {
    std::shared_ptr<SamplerState> sampler;
    std::uint32_t slotIndex;

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
    std::uint32_t slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setTexture(slotIndex, texture);
    }
};

struct SetTextureRenderTarget2DCommand final : public GraphicsCommand {
    std::shared_ptr<RenderTarget2D> texture;
    std::uint32_t slotIndex;

    void execute(GraphicsContext& graphicsContext) const override
    {
        graphicsContext.setTexture(slotIndex, texture);
    }
};

struct SetTextureIndexOnlyCommand final : public GraphicsCommand {
    std::uint32_t slotIndex;

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
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(vertexCount >= 1);
    auto command = memory::placementNew<DrawCommand>(allocator_);
    command->vertexCount = static_cast<std::uint32_t>(vertexCount);
    command->startVertexLocation = static_cast<std::uint32_t>(startVertexLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(indexCount >= 1);
    auto command = memory::placementNew<DrawIndexedCommand>(allocator_);
    command->indexCount = static_cast<std::uint32_t>(indexCount);
    command->startIndexLocation = static_cast<std::uint32_t>(startIndexLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(vertexCountPerInstance >= 1);
    auto command = memory::placementNew<DrawInstancedCommand>(allocator_);
    command->vertexCountPerInstance = static_cast<std::uint32_t>(vertexCountPerInstance);
    command->instanceCount = static_cast<std::uint32_t>(instanceCount);
    command->startVertexLocation = static_cast<std::uint32_t>(startVertexLocation);
    command->startInstanceLocation = static_cast<std::uint32_t>(startInstanceLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::drawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    flushRenderPassCommands();

    POMDOG_ASSERT(indexCountPerInstance >= 1);
    auto command = memory::placementNew<DrawIndexedInstancedCommand>(allocator_);
    command->indexCountPerInstance = static_cast<std::uint32_t>(indexCountPerInstance);
    command->instanceCount = static_cast<std::uint32_t>(instanceCount);
    command->startIndexLocation = static_cast<std::uint32_t>(startIndexLocation);
    command->startInstanceLocation = static_cast<std::uint32_t>(startInstanceLocation);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::setRenderPass(RenderPass&& renderPass)
{
    if (needToEndRenderPass_) {
        endRenderPass();
        needToEndRenderPass_ = false;
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
    auto command = memory::placementNew<EndRenderPassCommand>(allocator_);
    commands_.push_back(std::move(command));
}

void CommandListImmediate::setViewport(const Viewport& viewport)
{
    auto command = memory::placementNew<SetViewportCommand>(allocator_);
    command->viewport = viewport;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setScissorRect(const Rectangle& scissorRect)
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
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    setVertexBuffer(index, vertexBuffer, 0);
}

void CommandListImmediate::setVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(vertexBuffer != nullptr);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");
    auto command = memory::placementNew<SetVertexBufferCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->vertexBuffer = vertexBuffer;
    command->offset = static_cast<std::uint32_t>(offset);
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
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    setConstantBuffer(index, constantBuffer, 0);
}

void CommandListImmediate::setConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");

    std::shared_ptr<Buffer> nativeBuffer{constantBuffer, constantBuffer->getBuffer()};

    auto command = memory::placementNew<SetConstantBufferCommand>(allocator_);
    command->constantBuffer = nativeBuffer;
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->offset = static_cast<std::uint32_t>(offset);
    command->sizeInBytes = static_cast<std::uint32_t>(constantBuffer->getSizeInBytes() - offset);
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(samplerState != nullptr);
    auto command = memory::placementNew<SetSamplerStateCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->sampler = samplerState;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    auto command = memory::placementNew<SetTextureIndexOnlyCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture != nullptr);
    auto command = memory::placementNew<SetTextureCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
    command->texture = texture;
    renderPassCommands_.push_back(std::move(command));
}

void CommandListImmediate::setTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture != nullptr);
    auto command = memory::placementNew<SetTextureRenderTarget2DCommand>(allocator_);
    command->slotIndex = static_cast<std::uint32_t>(index);
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
