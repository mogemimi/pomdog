// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandListImmediate.hpp"
#include "NativeGraphicsContext.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace {

using Detail::GraphicsCommand;

struct DrawCommand final: public GraphicsCommand {
    std::size_t vertexCount;
    std::size_t startVertexLocation;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.Draw(vertexCount, startVertexLocation);
    }
};

struct DrawIndexedCommand final: public GraphicsCommand {
    std::size_t indexCount;
    std::size_t startIndexLocation;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawIndexed(indexCount, startIndexLocation);
    }
};

struct DrawInstancedCommand final: public GraphicsCommand {
    std::size_t vertexCountPerInstance;
    std::size_t instanceCount;
    std::size_t startVertexLocation;
    std::size_t startInstanceLocation;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawInstanced(
            vertexCountPerInstance,
            instanceCount,
            startVertexLocation,
            startInstanceLocation);
    }
};

struct DrawIndexedInstancedCommand final: public GraphicsCommand {
    std::size_t indexCountPerInstance;
    std::size_t instanceCount;
    std::size_t startIndexLocation;
    std::size_t startInstanceLocation;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawIndexedInstanced(
            indexCountPerInstance,
            instanceCount,
            startIndexLocation,
            startInstanceLocation);
    }
};

struct SetPrimitiveTopologyCommand final: public GraphicsCommand {
    PrimitiveTopology primitiveTopology;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.SetPrimitiveTopology(primitiveTopology);
    }
};

struct SetBlendFactorCommand final: public GraphicsCommand {
    Color blendFactor;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.SetBlendFactor(blendFactor);
    }
};

struct SetVertexBuffersCommand final: public GraphicsCommand {
    std::vector<VertexBufferBinding> vertexBuffers;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.SetVertexBuffers(vertexBuffers);
    }
};

struct SetIndexBufferCommand final: public GraphicsCommand {
    std::shared_ptr<IndexBuffer> indexBuffer;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.SetIndexBuffer(indexBuffer);
    }
};

struct SetPipelineStateCommand final: public GraphicsCommand {
    std::shared_ptr<NativePipelineState> pipelineState;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(pipelineState);
        graphicsContext.SetPipelineState(pipelineState);
    }
};

struct SetConstantBufferCommand final: public GraphicsCommand {
    std::shared_ptr<NativeBuffer> constantBuffer;
    int slotIndex;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(constantBuffer);
        POMDOG_ASSERT(slotIndex >= 0);
        graphicsContext.SetConstantBuffer(slotIndex, constantBuffer);
    }
};

struct SetSamplerStateCommand final: public GraphicsCommand {
    std::shared_ptr<NativeSamplerState> sampler;
    int slotIndex;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        if (sampler) {
            graphicsContext.SetSampler(slotIndex, sampler);
        }
        else {
            graphicsContext.SetSampler(slotIndex, sampler);
        }
    }
};

struct SetTextureCommand final: public GraphicsCommand {
    std::shared_ptr<Texture2D> texture;
    int slotIndex;

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        if (texture) {
            graphicsContext.SetTexture(slotIndex, texture);
        }
        else {
            graphicsContext.SetTexture(slotIndex);
        }
    }
};

struct SetTextureRenderTarget2DCommand final: public GraphicsCommand {
    std::shared_ptr<RenderTarget2D> texture;
    int slotIndex;

    void Execute(NativeGraphicsContext & graphicsContext) override
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

    void Execute(NativeGraphicsContext & graphicsContext) override
    {
        graphicsContext.SetRenderPass(renderPass);
    }
};

} // unnamed namespace

GraphicsCommandListImmediate::~GraphicsCommandListImmediate() = default;

void GraphicsCommandListImmediate::Close()
{
#if defined(__APPLE_CC__)
    constexpr bool useMetal = true;
#else
    constexpr bool useMetal = false;
#endif
    if (useMetal) {
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

void GraphicsCommandListImmediate::SetRenderPass(RenderPass && renderPass)
{
    auto command = std::make_unique<SetRenderPassCommand>();
    command->commandType = GraphicsCommandType::SetRenderPassCommand;
    command->renderPass = std::move(renderPass);

    if (command->renderPass.RenderTargets.empty()) {
        command->renderPass.RenderTargets.emplace_back(nullptr, Pomdog::NullOpt);
    }
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    auto command = std::make_unique<SetPrimitiveTopologyCommand>();
    command->commandType = GraphicsCommandType::SetPrimitiveTopologyCommand;
    command->primitiveTopology = primitiveTopology;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetBlendFactor(const Color& blendFactor)
{
    auto command = std::make_unique<SetBlendFactorCommand>();
    command->commandType = GraphicsCommandType::SetBlendFactorCommand;
    command->blendFactor = blendFactor;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<SetVertexBuffersCommand>();
    command->commandType = GraphicsCommandType::SetVertexBuffersCommand;
    command->vertexBuffers = vertexBuffers;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<SetVertexBuffersCommand>();
    command->commandType = GraphicsCommandType::SetVertexBuffersCommand;
    command->vertexBuffers = std::move(vertexBuffers);
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer);
    auto command = std::make_unique<SetIndexBufferCommand>();
    command->commandType = GraphicsCommandType::SetIndexBufferCommand;
    command->indexBuffer = indexBuffer;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = std::make_unique<SetPipelineStateCommand>();
    command->commandType = GraphicsCommandType::SetPipelineStateCommand;
    command->pipelineState = pipelineState;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    auto command = std::make_unique<SetConstantBufferCommand>();
    command->commandType = GraphicsCommandType::SetConstantBufferCommand;
    command->constantBuffer = constantBuffer;
    command->slotIndex = index;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(sampler);
    auto command = std::make_unique<SetSamplerStateCommand>();
    command->commandType = GraphicsCommandType::SetSamplerStateCommand;
    command->slotIndex = index;
    command->sampler = std::move(sampler);
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

void GraphicsCommandListImmediate::ExecuteImmediate(NativeGraphicsContext & graphicsContext)
{
    for (auto & command : commands) {
        POMDOG_ASSERT(command != nullptr);
        command->Execute(graphicsContext);
    }
}

void GraphicsCommandListImmediate::SortCommandsForMetal()
{
    // NOTE: This function do sorting, duplicating and removing commands for MTLRenderCommandEncoder.

    std::shared_ptr<GraphicsCommand> setPipelineStateCommand;
    std::shared_ptr<GraphicsCommand> setPrimitiveTopologyCommand;
    std::shared_ptr<GraphicsCommand> setBlendFactorCommand;
    std::shared_ptr<GraphicsCommand> setVertexBuffersCommand;
    std::shared_ptr<GraphicsCommand> setIndexBufferCommand;
    std::vector<std::shared_ptr<GraphicsCommand>> setConstantBufferCommands;
    std::vector<std::shared_ptr<GraphicsCommand>> setSamplerCommands;
    std::vector<std::shared_ptr<GraphicsCommand>> setTextureCommands;

    setConstantBufferCommands.resize(8, nullptr);
    setSamplerCommands.resize(8, nullptr);
    setTextureCommands.resize(8, nullptr);

    std::vector<std::shared_ptr<GraphicsCommand>> newCommands;
    std::size_t index = 0;
    std::size_t lastDrawCommandIndex = 0;
    bool needToFlushCommands = true;

    for (auto & command : commands) {
        POMDOG_ASSERT(command != nullptr);
        switch (command->commandType) {
        case GraphicsCommandType::SetRenderPassCommand:
            newCommands.push_back(std::move(command));
            needToFlushCommands = true;
            break;
        case GraphicsCommandType::SetPipelineStateCommand:
            setPipelineStateCommand = command;
            break;
        case GraphicsCommandType::SetPrimitiveTopologyCommand:
            setPrimitiveTopologyCommand = command;
            break;
        case GraphicsCommandType::SetBlendFactorCommand:
            setBlendFactorCommand = command;
            break;
        case GraphicsCommandType::SetVertexBuffersCommand:
            setVertexBuffersCommand = command;
            break;
        case GraphicsCommandType::SetIndexBufferCommand:
            setIndexBufferCommand = command;
            break;
        case GraphicsCommandType::SetConstantBufferCommand: {
            auto c = std::static_pointer_cast<SetConstantBufferCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setConstantBufferCommands.size()));
            setConstantBufferCommands[c->slotIndex] = c;
            break;
        }
        case GraphicsCommandType::SetSamplerStateCommand: {
            auto c = std::static_pointer_cast<SetSamplerStateCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setSamplerCommands.size()));
            setSamplerCommands[c->slotIndex] = c;
            break;
        }
        case GraphicsCommandType::SetTextureCommand: {
            auto c = std::static_pointer_cast<SetTextureCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setTextureCommands.size()));
            setTextureCommands[c->slotIndex] = c;
            break;
        }
        case GraphicsCommandType::SetTextureRenderTarget2DCommand: {
            auto c = std::static_pointer_cast<SetTextureRenderTarget2DCommand>(command);
            POMDOG_ASSERT(c->slotIndex < static_cast<int>(setTextureCommands.size()));
            setTextureCommands[c->slotIndex] = c;
            break;
        }
        case GraphicsCommandType::DrawCommand:
        case GraphicsCommandType::DrawIndexedCommand:
        case GraphicsCommandType::DrawInstancedCommand:
        case GraphicsCommandType::DrawIndexedInstancedCommand: {
            if (needToFlushCommands) {
                if (setPipelineStateCommand) {
                    newCommands.push_back(setPipelineStateCommand);
                }
                if (setPrimitiveTopologyCommand) {
                    newCommands.push_back(setPrimitiveTopologyCommand);
                }
                if (setBlendFactorCommand) {
                    newCommands.push_back(setBlendFactorCommand);
                }
                if (setVertexBuffersCommand) {
                    newCommands.push_back(setVertexBuffersCommand);
                }
                if (setIndexBufferCommand) {
                    newCommands.push_back(setIndexBufferCommand);
                }
                for (auto & c : setConstantBufferCommands) {
                    if (c) {
                        newCommands.push_back(c);
                    }
                }
                for (auto & c : setSamplerCommands) {
                    if (c) {
                        newCommands.push_back(c);
                    }
                }
                for (auto & c : setTextureCommands) {
                    if (c) {
                        newCommands.push_back(c);
                    }
                }
                needToFlushCommands = false;
            }
            newCommands.push_back(std::move(command));
            lastDrawCommandIndex = index;
            break;
        }
        }
        ++index;
    }

    for (auto i = lastDrawCommandIndex + 1; i < commands.size(); ++i) {
        newCommands.push_back(std::move(commands[i]));
    }

    std::swap(newCommands, commands);
}

} // namespace Detail
} // namespace Pomdog
