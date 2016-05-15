// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
    GraphicsCommandListImmediate(const GraphicsCommandListImmediate&) = delete;
    GraphicsCommandListImmediate & operator=(const GraphicsCommandListImmediate&) = delete;

    ~GraphicsCommandListImmediate();

    void Close() override;

    void Reset() override;

    std::size_t GetCount() const noexcept override;

    void Clear(
        ClearOptions options,
        const Color& color,
        float depth,
        std::uint8_t stencil) override;

    void Draw(std::size_t vertexCount) override;

    void DrawIndexed(std::size_t indexCount) override;

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) override;

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) override;

    void SetViewport(const Viewport& viewport) override;

    void SetScissorRectangle(const Rectangle& rectangle) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(const Color& blendFactor) override;

    void SetVertexBuffers(
        const std::vector<VertexBufferBinding>& vertexBuffers) override;

    void SetVertexBuffers(
        std::vector<VertexBufferBinding> && vertexBuffers) override;

    void SetIndexBuffer(
        const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(
        const std::shared_ptr<NativePipelineState>& pipelineState) override;

    void SetConstantBuffer(
        int index, const std::shared_ptr<NativeBuffer>& constantBuffer) override;

    void SetSampler(
        int index, std::shared_ptr<NativeSamplerState> && sampler) override;

    void SetTexture(int index) override;

    void SetTexture(
        int index, const std::shared_ptr<Texture2D>& texture) override;

    void SetTexture(
        int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(
        const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets) override;

    void SetRenderTargets(
        std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets) override;

    void ExecuteImmediate(GraphicsContext & graphicsContext) const;

private:
    std::vector<std::unique_ptr<Detail::GraphicsCommand>> commands;
};

} // namespace Detail
} // namespace Pomdog
