// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/graphics_command_list_immediate.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/native_graphics_context.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

using detail::GraphicsCommand;

struct DrawCommand final : public GraphicsCommand {
    std::size_t vertexCount;
    std::size_t startVertexLocation;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.Draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final : public GraphicsCommand {
    std::size_t indexCount;
    std::size_t startIndexLocation;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.DrawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final : public GraphicsCommand {
    std::size_t vertexCountPerInstance;
    std::size_t instanceCount;
    std::size_t startVertexLocation;
    std::size_t startInstanceLocation;

    void Execute(NativeGraphicsContext& graphicsContext) override
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

    void Execute(NativeGraphicsContext& graphicsContext) override
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

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetViewport(viewport);
    }
};

struct SetScissorRectCommand final : public GraphicsCommand {
    Rectangle scissorRect;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetScissorRect(scissorRect);
    }
};

struct SetBlendFactorCommand final : public GraphicsCommand {
    Vector4 blendFactor;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetBlendFactor(blendFactor);
    }
};

struct SetVertexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<VertexBuffer> vertexBuffer;
    int slotIndex;
    std::size_t offset;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetVertexBuffer(slotIndex, vertexBuffer, offset);
    }
};

struct SetIndexBufferCommand final : public GraphicsCommand {
    std::shared_ptr<IndexBuffer> indexBuffer;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetIndexBuffer(indexBuffer);
    }
};

struct SetPipelineStateCommand final : public GraphicsCommand {
    std::shared_ptr<PipelineState> pipelineState;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        POMDOG_ASSERT(pipelineState);
        graphicsContext.SetPipelineState(pipelineState);
    }
};

struct SetConstantBufferCommand final : public GraphicsCommand {
    std::shared_ptr<NativeBuffer> constantBuffer;
    int slotIndex;
    std::size_t offset;
    std::size_t sizeInBytes;

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        POMDOG_ASSERT(constantBuffer);
        POMDOG_ASSERT(slotIndex >= 0);
        graphicsContext.SetConstantBuffer(slotIndex, constantBuffer, offset, sizeInBytes);
    }
};

struct SetSamplerStateCommand final : public GraphicsCommand {
    std::shared_ptr<SamplerState> sampler;
    int slotIndex;

    void Execute(NativeGraphicsContext& graphicsContext) override
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
    std::shared_ptr<Texture2D> texture;
    int slotIndex;

    void Execute(NativeGraphicsContext& graphicsContext) override
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

    void Execute(NativeGraphicsContext& graphicsContext) override
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

    void Execute(NativeGraphicsContext& graphicsContext) override
    {
        graphicsContext.SetRenderPass(renderPass);
    }
};

} // namespace

GraphicsCommand::~GraphicsCommand() = default;

GraphicsCommandListImmediate::~GraphicsCommandListImmediate() = default;

void GraphicsCommandListImmediate::Close()
{
#if defined(__APPLE_CC__)
    constexpr bool useMetal = true;
#else
    constexpr bool useMetal = false;
#endif
    if constexpr (useMetal) {
        SortCommandsForMetal();
    }
    commands.shrink_to_fit();
}

void GraphicsCommandListImmediate::Reset()
{
    commands.clear();
}

std::size_t GraphicsCommandListImmediate::GetCount() const noexcept
{
    return commands.size();
}

void GraphicsCommandListImmediate::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<DrawCommand>();
    command->commandType = GraphicsCommandType::DrawCommand;
    command->vertexCount = vertexCount;
    command->startVertexLocation = startVertexLocation;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<DrawIndexedCommand>();
    command->commandType = GraphicsCommandType::DrawIndexedCommand;
    command->indexCount = indexCount;
    command->startIndexLocation = startIndexLocation;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(vertexCountPerInstance >= 1);
    auto command = std::make_unique<DrawInstancedCommand>();
    command->commandType = GraphicsCommandType::DrawInstancedCommand;
    command->vertexCountPerInstance = vertexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startVertexLocation = startVertexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(indexCountPerInstance >= 1);
    auto command = std::make_unique<DrawIndexedInstancedCommand>();
    command->commandType = GraphicsCommandType::DrawIndexedInstancedCommand;
    command->indexCountPerInstance = indexCountPerInstance;
    command->instanceCount = instanceCount;
    command->startIndexLocation = startIndexLocation;
    command->startInstanceLocation = startInstanceLocation;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetRenderPass(RenderPass&& renderPass)
{
    auto command = std::make_unique<SetRenderPassCommand>();
    command->commandType = GraphicsCommandType::SetRenderPassCommand;
    command->renderPass = std::move(renderPass);

    if (command->renderPass.RenderTargets.empty()) {
        command->renderPass.RenderTargets[0] = {nullptr, std::nullopt};
    }
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetViewport(const Viewport& viewport)
{
    auto command = std::make_unique<SetViewportCommand>();
    command->commandType = GraphicsCommandType::SetViewportCommand;
    command->viewport = viewport;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetScissorRect(const Rectangle& scissorRect)
{
    auto command = std::make_unique<SetScissorRectCommand>();
    command->commandType = GraphicsCommandType::SetScissorRectCommand;
    command->scissorRect = scissorRect;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetBlendFactor(const Vector4& blendFactor)
{
    auto command = std::make_unique<SetBlendFactorCommand>();
    command->commandType = GraphicsCommandType::SetBlendFactorCommand;
    command->blendFactor = blendFactor;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    SetVertexBuffer(index, vertexBuffer, 0);
}

void GraphicsCommandListImmediate::SetVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(vertexBuffer != nullptr);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");
    auto command = std::make_unique<SetVertexBufferCommand>();
    command->commandType = GraphicsCommandType::SetVertexBufferCommand;
    command->slotIndex = index;
    command->vertexBuffer = vertexBuffer;
    command->offset = offset;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer != nullptr);
    auto command = std::make_unique<SetIndexBufferCommand>();
    command->commandType = GraphicsCommandType::SetIndexBufferCommand;
    command->indexBuffer = indexBuffer;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = std::make_unique<SetPipelineStateCommand>();
    command->commandType = GraphicsCommandType::SetPipelineStateCommand;
    command->pipelineState = pipelineState;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);

    std::shared_ptr<NativeBuffer> nativeBuffer(constantBuffer, constantBuffer->GetNativeBuffer());

    auto command = std::make_unique<SetConstantBufferCommand>();
    command->commandType = GraphicsCommandType::SetConstantBufferCommand;
    command->constantBuffer = nativeBuffer;
    command->slotIndex = index;
    command->offset = 0;
    command->sizeInBytes = constantBuffer->GetSizeInBytes();
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetConstantBuffer(
    int index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");

    std::shared_ptr<NativeBuffer> nativeBuffer(constantBuffer, constantBuffer->GetNativeBuffer());

    auto command = std::make_unique<SetConstantBufferCommand>();
    command->commandType = GraphicsCommandType::SetConstantBufferCommand;
    command->constantBuffer = nativeBuffer;
    command->slotIndex = index;
    command->offset = offset;
    command->sizeInBytes = constantBuffer->GetSizeInBytes() - offset;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(samplerState != nullptr);
    auto command = std::make_unique<SetSamplerStateCommand>();
    command->commandType = GraphicsCommandType::SetSamplerStateCommand;
    command->slotIndex = index;
    command->sampler = samplerState;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    auto command = std::make_unique<SetTextureCommand>();
    command->commandType = GraphicsCommandType::SetTextureCommand;
    command->slotIndex = index;
    command->texture = nullptr;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index, const std::shared_ptr<Texture2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureCommand>();
    command->commandType = GraphicsCommandType::SetTextureCommand;
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureRenderTarget2DCommand>();
    command->commandType = GraphicsCommandType::SetTextureRenderTarget2DCommand;
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::ExecuteImmediate(NativeGraphicsContext& graphicsContext)
{
    for (auto& command : commands) {
        POMDOG_ASSERT(command != nullptr);
        command->Execute(graphicsContext);
    }
}

void GraphicsCommandListImmediate::SortCommandsForMetal()
{
    static_assert(static_cast<int>(GraphicsCommandType::DrawCommand) == 0);
    static_assert(static_cast<int>(GraphicsCommandType::DrawIndexedCommand) == 1);
    static_assert(static_cast<int>(GraphicsCommandType::DrawInstancedCommand) == 2);
    static_assert(static_cast<int>(GraphicsCommandType::DrawIndexedInstancedCommand) == 3);
    static_assert(static_cast<int>(GraphicsCommandType::SetRenderPassCommand) == 4);
    static_assert(static_cast<int>(GraphicsCommandType::SetViewportCommand) == 5);
    static_assert(static_cast<int>(GraphicsCommandType::SetScissorRectCommand) == 6);
    static_assert(static_cast<int>(GraphicsCommandType::SetBlendFactorCommand) == 7);
    static_assert(static_cast<int>(GraphicsCommandType::SetVertexBufferCommand) == 8);
    static_assert(static_cast<int>(GraphicsCommandType::SetIndexBufferCommand) == 9);
    static_assert(static_cast<int>(GraphicsCommandType::SetPipelineStateCommand) == 10);
    static_assert(static_cast<int>(GraphicsCommandType::SetConstantBufferCommand) == 11);
    static_assert(static_cast<int>(GraphicsCommandType::SetSamplerStateCommand) == 12);
    static_assert(static_cast<int>(GraphicsCommandType::SetTextureCommand) == 13);
    static_assert(static_cast<int>(GraphicsCommandType::SetTextureRenderTarget2DCommand) == 14);

    constexpr std::int8_t priorityDrawCommand = 31;
    constexpr std::int8_t priorityDefault = 30;

    const std::array<std::int8_t, 15> priorities = {{
        priorityDrawCommand, // DrawCommand
        priorityDrawCommand, // DrawIndexedCommand
        priorityDrawCommand, // DrawInstancedCommand
        priorityDrawCommand, // DrawIndexedInstancedCommand
        0, // SetRenderPassCommand
        priorityDefault, // SetViewportCommand
        priorityDefault, // SetScissorRectCommand
        priorityDefault, // SetBlendFactorCommand
        priorityDefault, // SetVertexBuffersCommand
        priorityDefault, // SetIndexBufferCommand
        priorityDefault, // SetPipelineStateCommand
        priorityDefault, // SetConstantBufferCommand
        priorityDefault, // SetSamplerStateCommand
        priorityDefault, // SetTextureCommand
        priorityDefault, // SetTextureRenderTarget2DCommand
    }};

    // NOTE: Sort commands for MTLRenderCommandEncoder by using odd-even sort.
    for (size_t k = 0; k < commands.size(); ++k) {
        bool swapped = false;
        for (size_t i = 1 + (k % 2); i < commands.size(); i += 2) {
            auto& a = commands[i - 1];
            auto& b = commands[i];
            const auto x = static_cast<std::int8_t>(a->commandType);
            const auto y = static_cast<std::int8_t>(b->commandType);
            POMDOG_ASSERT(x <= static_cast<std::int8_t>(priorities.size()));
            POMDOG_ASSERT(y <= static_cast<std::int8_t>(priorities.size()));
            if ((priorities[x] > priorities[y]) && (priorities[x] != priorityDrawCommand)) {
                std::swap(a, b);
                swapped = true;
            }
        }
        if (!swapped && (k > 0)) {
            break;
        }
    }

#if 0
    // NOTE: Remove a redundant 'SetRenderPassCommand' command.
    bool isPrevRenderPassCommand = false;
    for (auto iter = commands.rbegin(); iter != commands.rend();) {
        if ((*iter)->commandType == GraphicsCommandType::SetRenderPassCommand) {
            if (isPrevRenderPassCommand) {
                std::advance(iter, 1);
                commands.erase(iter.base());
                continue;
            }
            isPrevRenderPassCommand = true;
        }
        else {
            isPrevRenderPassCommand = false;
        }
        ++iter;
    }
#endif

    // NOTE: Duplicate missing commands for MTLRenderCommandEncoder.
    std::size_t renderPassCommandIndex = 0;
    std::optional<std::size_t> setPipelineStateCommand;
    std::optional<std::size_t> setPrimitiveTopologyCommand;
    std::optional<std::size_t> setViewportCommand;
    std::optional<std::size_t> setScissorRectCommand;
    std::optional<std::size_t> setBlendFactorCommand;
    std::optional<std::size_t> setIndexBufferCommand;
    std::array<std::optional<std::size_t>, 8> setVertexBufferCommands;
    std::array<std::optional<std::size_t>, 8> setConstantBufferCommands;
    std::array<std::optional<std::size_t>, 8> setSamplerCommands;
    std::array<std::optional<std::size_t>, 8> setTextureCommands;

    std::vector<std::shared_ptr<GraphicsCommand>> oldCommands;
    bool needToFlushCommands = true;

    std::swap(commands, oldCommands);

    for (auto& command : oldCommands) {
        POMDOG_ASSERT(command != nullptr);

        bool isDrawCommand = false;

        switch (command->commandType) {
        case GraphicsCommandType::SetRenderPassCommand:
            renderPassCommandIndex = commands.size();
            needToFlushCommands = true;
            break;
        case GraphicsCommandType::SetPipelineStateCommand:
            setPipelineStateCommand = commands.size();
            break;
        case GraphicsCommandType::SetViewportCommand:
            setViewportCommand = commands.size();
            break;
        case GraphicsCommandType::SetScissorRectCommand:
            setScissorRectCommand = commands.size();
            break;
        case GraphicsCommandType::SetBlendFactorCommand:
            setBlendFactorCommand = commands.size();
            break;
        case GraphicsCommandType::SetIndexBufferCommand:
            setIndexBufferCommand = commands.size();
            break;
        case GraphicsCommandType::SetVertexBufferCommand: {
            auto c = std::static_pointer_cast<SetVertexBufferCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setVertexBufferCommands.size()));
            setVertexBufferCommands[c->slotIndex] = commands.size();
            break;
        }
        case GraphicsCommandType::SetConstantBufferCommand: {
            auto c = std::static_pointer_cast<SetConstantBufferCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setConstantBufferCommands.size()));
            setConstantBufferCommands[c->slotIndex] = commands.size();
            break;
        }
        case GraphicsCommandType::SetSamplerStateCommand: {
            auto c = std::static_pointer_cast<SetSamplerStateCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setSamplerCommands.size()));
            setSamplerCommands[c->slotIndex] = commands.size();
            break;
        }
        case GraphicsCommandType::SetTextureCommand: {
            auto c = std::static_pointer_cast<SetTextureCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setTextureCommands.size()));
            setTextureCommands[c->slotIndex] = commands.size();
            break;
        }
        case GraphicsCommandType::SetTextureRenderTarget2DCommand: {
            auto c = std::static_pointer_cast<SetTextureRenderTarget2DCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setTextureCommands.size()));
            setTextureCommands[c->slotIndex] = commands.size();
            break;
        }
        case GraphicsCommandType::DrawCommand:
        case GraphicsCommandType::DrawIndexedCommand:
        case GraphicsCommandType::DrawInstancedCommand:
        case GraphicsCommandType::DrawIndexedInstancedCommand: {
            isDrawCommand = true;
            break;
        }
        }

        if (isDrawCommand && needToFlushCommands) {
            if (setPipelineStateCommand && (*setPipelineStateCommand < renderPassCommandIndex)) {
                commands.push_back(commands[*setPipelineStateCommand]);
            }
            if (setPrimitiveTopologyCommand && (*setPrimitiveTopologyCommand < renderPassCommandIndex)) {
                commands.push_back(commands[*setPrimitiveTopologyCommand]);
            }
            if (setScissorRectCommand && (*setScissorRectCommand < renderPassCommandIndex)) {
                commands.push_back(commands[*setScissorRectCommand]);
            }
            if (setBlendFactorCommand && (*setBlendFactorCommand < renderPassCommandIndex)) {
                commands.push_back(commands[*setBlendFactorCommand]);
            }
            if (setIndexBufferCommand && (*setIndexBufferCommand < renderPassCommandIndex)) {
                commands.push_back(commands[*setIndexBufferCommand]);
            }
            for (auto& commandIndex : setVertexBufferCommands) {
                if (commandIndex && (*commandIndex < renderPassCommandIndex)) {
                    commands.push_back(commands[*commandIndex]);
                }
            }
            for (auto& commandIndex : setConstantBufferCommands) {
                if (commandIndex && (*commandIndex < renderPassCommandIndex)) {
                    commands.push_back(commands[*commandIndex]);
                }
            }
            for (auto& commandIndex : setSamplerCommands) {
                if (commandIndex && (*commandIndex < renderPassCommandIndex)) {
                    commands.push_back(commands[*commandIndex]);
                }
            }
            for (auto& commandIndex : setTextureCommands) {
                if (commandIndex && (*commandIndex < renderPassCommandIndex)) {
                    commands.push_back(commands[*commandIndex]);
                }
            }
            needToFlushCommands = false;
        }
        commands.push_back(std::move(command));
    }
}

} // namespace pomdog::detail
