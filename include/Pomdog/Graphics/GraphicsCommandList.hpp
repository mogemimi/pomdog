// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT GraphicsCommandList {
public:
    GraphicsCommandList() = delete;
    GraphicsCommandList(GraphicsCommandList const&) = delete;
    GraphicsCommandList & operator=(GraphicsCommandList const&) = delete;

    explicit GraphicsCommandList(GraphicsDevice & graphicsDevice);

    explicit GraphicsCommandList(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    ~GraphicsCommandList();

    void Close();

    void Reset();

    std::size_t GetCount() const noexcept;

    void Clear(Color const& color);

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil);

    void Draw(std::size_t vertexCount);

    void DrawIndexed(
        std::shared_ptr<IndexBuffer> const& indexBuffer,
        std::size_t indexCount);

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount);

    void DrawIndexedInstanced(
        std::shared_ptr<IndexBuffer> const& indexBuffer,
        std::size_t indexCount,
        std::size_t instanceCount);

    void SetViewport(Viewport const& viewport);

    void SetScissorRectangle(Rectangle const& rectangle);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(Color const& blendFactor);

    void SetVertexBuffer(std::shared_ptr<VertexBuffer> const& vertexBuffer);

    void SetVertexBuffer(
        std::shared_ptr<VertexBuffer> const& vertexBuffer,
        std::size_t offset);

    void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers);

    void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers);

    void SetPipelineState(std::shared_ptr<PipelineState> const& pipelineState);

    void SetConstantBuffer(int index, std::shared_ptr<ConstantBuffer> const& constantBuffer);

    void SetTexture(int index);

    void SetTexture(int index, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture);

    void SetSamplerState(int index, std::shared_ptr<SamplerState> const& samplerState);

    void SetRenderTarget();

    void SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    Detail::NativeGraphicsCommandList* GetNativeGraphicsCommandList();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandList> nativeCommandList;
};

} // namespace Pomdog
