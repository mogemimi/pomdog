// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsCommandListImmediate.hpp"
#include "GraphicsContext.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace {
namespace Commands {

using Detail::GraphicsCommand;

struct ClearCommand final: public GraphicsCommand {
    Color color;
    float depth;
    std::uint8_t stencil;
    ClearOptions options;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.Clear(options, color, depth, stencil);
    }
};

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

struct SetViewportCommand final: public GraphicsCommand {
    Pomdog::Viewport viewport;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetViewport(viewport);
    }
};

struct SetScissorRectangleCommand final: public GraphicsCommand {
    Pomdog::Rectangle rectangle;

    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetScissorRectangle(rectangle);
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

struct SetConstantBuffersCommand final: public GraphicsCommand {
    std::shared_ptr<NativeConstantLayout> constantBuffers;

    void Execute(GraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(constantBuffers);
        graphicsContext.SetConstantBuffers(constantBuffers);
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

struct SetBackBufferAsRenderTargetCommand final: public GraphicsCommand {
    void Execute(GraphicsContext & graphicsContext) override
    {
        graphicsContext.SetRenderTarget();
    }
};

struct SetRenderTargetsCommand final: public GraphicsCommand {
    std::vector<std::shared_ptr<RenderTarget2D>> renderTargets;

    void Execute(GraphicsContext & graphicsContext) override
    {
        POMDOG_ASSERT(!renderTargets.empty());
        graphicsContext.SetRenderTargets(renderTargets);
    }
};

} // namespace Commands
} // unnamed namespace
//-----------------------------------------------------------------------
GraphicsCommandListImmediate::~GraphicsCommandListImmediate() = default;
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::Close()
{
    //commands.shrink_to_fit();
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::Reset()
{
    commands.clear();
}
//-----------------------------------------------------------------------
std::size_t GraphicsCommandListImmediate::GetCount() const noexcept
{
    return commands.size();
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil)
{
    auto command = std::make_unique<Commands::ClearCommand>();
    command->options = options;
    command->color = color;
    command->depth = depth;
    command->stencil = stencil;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::Draw(std::size_t vertexCount)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<Commands::DrawCommand>();
    command->vertexCount = vertexCount;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::DrawIndexed(
    std::size_t indexCount)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<Commands::DrawIndexedCommand>();
    command->indexCount = indexCount;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::DrawInstanced(
    std::size_t vertexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(vertexCount >= 1);
    auto command = std::make_unique<Commands::DrawInstancedCommand>();
    command->vertexCount = vertexCount;
    command->instanceCount = instanceCount;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::DrawIndexedInstanced(
    std::size_t indexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(indexCount >= 1);
    auto command = std::make_unique<Commands::DrawIndexedInstancedCommand>();
    command->indexCount = indexCount;
    command->instanceCount = instanceCount;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetViewport(Viewport const& viewport)
{
    auto command = std::make_unique<Commands::SetViewportCommand>();
    command->viewport = viewport;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetScissorRectangle(Rectangle const& rectangle)
{
    auto command = std::make_unique<Commands::SetScissorRectangleCommand>();
    command->rectangle = rectangle;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    auto command = std::make_unique<Commands::SetPrimitiveTopologyCommand>();
    command->primitiveTopology = primitiveTopology;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetBlendFactor(Color const& blendFactor)
{
    auto command = std::make_unique<Commands::SetBlendFactorCommand>();
    command->blendFactor = blendFactor;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<Commands::SetVertexBuffersCommand>();
    command->vertexBuffers = vertexBuffers;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    auto command = std::make_unique<Commands::SetVertexBuffersCommand>();
    command->vertexBuffers = std::move(vertexBuffers);
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer);
    auto command = std::make_unique<Commands::SetIndexBufferCommand>();
    command->indexBuffer = indexBuffer;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    auto command = std::make_unique<Commands::SetPipelineStateCommand>();
    command->pipelineState = pipelineState;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantBuffers)
{
    POMDOG_ASSERT(constantBuffers);
    auto command = std::make_unique<Commands::SetConstantBuffersCommand>();
    command->constantBuffers = constantBuffers;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler)
{
    POMDOG_ASSERT(sampler);
    auto command = std::make_unique<Commands::SetSamplerStateCommand>();
    command->slotIndex = index;
    command->sampler = std::move(sampler);
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetTexture(int index)
{
    auto command = std::make_unique<Commands::SetTextureCommand>();
    command->slotIndex = index;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetTexture(int index, std::shared_ptr<Texture2D> const& texture)
{
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<Commands::SetTextureCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture)
{
    POMDOG_ASSERT(texture);
    auto command = std::make_unique<Commands::SetTextureRenderTarget2DCommand>();
    command->slotIndex = index;
    command->texture = texture;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetRenderTarget()
{
    auto command = std::make_unique<Commands::SetBackBufferAsRenderTargetCommand>();
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    auto command = std::make_unique<Commands::SetRenderTargetsCommand>();
    command->renderTargets = renderTargets;
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    auto command = std::make_unique<Commands::SetRenderTargetsCommand>();
    command->renderTargets = std::move(renderTargets);
    commands.push_back(std::move(command));
}
//-----------------------------------------------------------------------
void GraphicsCommandListImmediate::ExecuteImmediate(GraphicsContext & graphicsContext) const
{
    for (auto & command : commands) {
        POMDOG_ASSERT(command);
        command->Execute(graphicsContext);
    }
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
