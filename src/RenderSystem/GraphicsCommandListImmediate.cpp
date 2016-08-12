// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandListImmediate.hpp"
#include "GraphicsContext.hpp"
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

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.Draw(vertexCount);
    }
};

struct DrawIndexedCommand final: public GraphicsCommand {
    std::size_t indexCount;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawIndexed(indexCount);
    }
};

struct DrawInstancedCommand final: public GraphicsCommand {
    std::size_t vertexCount;
    std::size_t instanceCount;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawInstanced(vertexCount, instanceCount);
    }
};

struct DrawIndexedInstancedCommand final: public GraphicsCommand {
    std::size_t indexCount;
    std::size_t instanceCount;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.DrawIndexedInstanced(indexCount, instanceCount);
    }
};

struct SetPrimitiveTopologyCommand final: public GraphicsCommand {
    PrimitiveTopology primitiveTopology;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetPrimitiveTopology(primitiveTopology);
    }
};

struct SetBlendFactorCommand final: public GraphicsCommand {
    Color blendFactor;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetBlendFactor(blendFactor);
    }
};

struct SetVertexBuffersCommand final: public GraphicsCommand {
    std::vector<VertexBufferBinding> vertexBuffers;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetVertexBuffers(vertexBuffers);
    }
};

struct SetIndexBufferCommand final: public GraphicsCommand {
    std::shared_ptr<IndexBuffer> indexBuffer;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetIndexBuffer(indexBuffer);
    }
};

struct SetPipelineStateCommand final: public GraphicsCommand {
    std::shared_ptr<NativePipelineState> pipelineState;

    void Execute(GraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(pipelineState);
        graphicsContext.SetPipelineState(pipelineState);
    }
};

struct SetConstantBufferCommand final: public GraphicsCommand {
    std::shared_ptr<NativeBuffer> constantBuffer;
    int slotIndex;

    void Execute(GraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(constantBuffer);
        POMDOG_ASSERT(slotIndex >= 0);
        graphicsContext.SetConstantBuffer(slotIndex, constantBuffer);
    }
};

struct SetSamplerStateCommand final: public GraphicsCommand {
    std::shared_ptr<NativeSamplerState> sampler;
    int slotIndex;

    void Execute(GraphicsContext & graphicsContext) override
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

    void Execute(GraphicsContext & graphicsContext) override
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

    void Execute(GraphicsContext & graphicsContext) override
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

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetRenderPass(renderPass);
    }
};

} // unnamed namespace

GraphicsCommandListImmediate::~GraphicsCommandListImmediate() = default;

void GraphicsCommandListImmediate::Close()
{
    //commands.shrink_to_fit();
}

void GraphicsCommandListImmediate::Reset()
{
    commands.clear();
}

std::size_t GraphicsCommandListImmediate::GetCount() const noexcept
{
    return commands.size();
}

void GraphicsCommandListImmediate::Draw(std::size_t vertexCount)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<DrawCommand>();
    command->vertexCount = vertexCount;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawIndexed(
    std::size_t indexCount)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<DrawIndexedCommand>();
    command->indexCount = indexCount;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawInstanced(
    std::size_t vertexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<DrawInstancedCommand>();
    command->vertexCount = vertexCount;
    command->instanceCount = instanceCount;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::DrawIndexedInstanced(
    std::size_t indexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<DrawIndexedInstancedCommand>();
    command->indexCount = indexCount;
    command->instanceCount = instanceCount;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetRenderPass(RenderPass && renderPass)
{
    auto command = std::make_unique<SetRenderPassCommand>();
    command->renderPass = std::move(renderPass);

    if (command->renderPass.RenderTargets.empty()) {
        command->renderPass.RenderTargets.emplace_back(nullptr, Pomdog::NullOpt);
    }
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    auto command = std::make_unique<SetPrimitiveTopologyCommand>();
    command->primitiveTopology = primitiveTopology;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetBlendFactor(const Color& blendFactor)
{
    auto command = std::make_unique<SetBlendFactorCommand>();
    command->blendFactor = blendFactor;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<SetVertexBuffersCommand>();
    command->vertexBuffers = vertexBuffers;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<SetVertexBuffersCommand>();
    command->vertexBuffers = std::move(vertexBuffers);
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer);
    auto command = std::make_unique<SetIndexBufferCommand>();
    command->indexBuffer = indexBuffer;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = std::make_unique<SetPipelineStateCommand>();
    command->pipelineState = pipelineState;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    auto command = std::make_unique<SetConstantBufferCommand>();
    command->constantBuffer = constantBuffer;
    command->slotIndex = index;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(sampler);
    auto command = std::make_unique<SetSamplerStateCommand>();
    command->slotIndex = index;
    command->sampler = std::move(sampler);
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    auto command = std::make_unique<SetTextureCommand>();
    command->slotIndex = index;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index, const std::shared_ptr<Texture2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<SetTextureRenderTarget2DCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}

void GraphicsCommandListImmediate::ExecuteImmediate(GraphicsContext & graphicsContext) const
{
    for (auto & command : commands) {
        POMDOG_ASSERT(command);
        command->Execute(graphicsContext);
    }
}

} // namespace Detail
} // namespace Pomdog
