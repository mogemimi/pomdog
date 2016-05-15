// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT GraphicsCommandList final {
public:
    GraphicsCommandList() = delete;
    GraphicsCommandList(const GraphicsCommandList&) = delete;
    GraphicsCommandList & operator=(const GraphicsCommandList&) = delete;

    explicit GraphicsCommandList(GraphicsDevice & graphicsDevice);

    explicit GraphicsCommandList(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    ~GraphicsCommandList();

    void Close();

    void Reset();

    std::size_t GetCount() const noexcept;

    void Clear(const Color& color);

    void Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil);

    void Draw(std::size_t vertexCount);

    void DrawIndexed(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount);

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount);

    void DrawIndexedInstanced(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount,
        std::size_t instanceCount);

    void SetViewport(const Viewport& viewport);

    void SetScissorRectangle(const Rectangle& rectangle);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(const Color& blendFactor);

    void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

    void SetVertexBuffer(
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset);

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers);

    void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers);

    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState);

    void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer);

    void SetTexture(int index);

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture);

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture);

    void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState);

    void SetRenderTarget();

    void SetRenderTarget(const std::shared_ptr<RenderTarget2D>& renderTarget);

    void SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    Detail::NativeGraphicsCommandList* GetNativeGraphicsCommandList();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandList> nativeCommandList;
};

} // namespace Pomdog
