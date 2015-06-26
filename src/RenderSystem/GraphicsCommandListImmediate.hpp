// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCOMMANDLISTIMMEDIATE_DADAF84E_HPP
#define POMDOG_GRAPHICSCOMMANDLISTIMMEDIATE_DADAF84E_HPP

#include "NativeGraphicsCommandList.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class GraphicsContext;

class GraphicsCommand {
public:
    virtual ~GraphicsCommand() = default;

    virtual void Execute(GraphicsContext & graphicsContext) = 0;
};

class GraphicsCommandListImmediate final: public NativeGraphicsCommandList {
public:
    GraphicsCommandListImmediate() = default;
    GraphicsCommandListImmediate(GraphicsCommandListImmediate const&) = delete;
    GraphicsCommandListImmediate & operator=(GraphicsCommandListImmediate const&) = delete;

    ~GraphicsCommandListImmediate();

    void Close() override;

    void Reset() override;

    std::size_t GetCount() const noexcept override;

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil) override;

    void Draw(std::size_t vertexCount) override;

    void DrawIndexed(std::size_t indexCount) override;

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) override;

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) override;

    void SetViewport(Viewport const& viewport) override;

    void SetScissorRectangle(Rectangle const& rectangle) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(Color const& blendFactor) override;

    void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers) override;

    void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers) override;

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer) override;

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) override;

    void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayout) override;

    void SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, std::shared_ptr<Texture2D> const& texture) override;

    void SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets) override;

    void ExecuteImmediate(GraphicsContext & graphicsContext) const;

private:
    std::vector<std::unique_ptr<Detail::GraphicsCommand>> commands;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCOMMANDLISTIMMEDIATE_DADAF84E_HPP
