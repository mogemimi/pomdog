// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <memory>
#import <Metal/Metal.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

struct ClearCommandMetal {
    Color color;
    float depth;
    std::uint8_t stencil;
    ClearOptions options;
};

class GraphicsContextMetal final : public NativeGraphicsContext {
public:
    GraphicsContextMetal() = delete;

    explicit GraphicsContextMetal(id<MTLDevice> nativeDevice);

    ~GraphicsContextMetal();

    GraphicsCapabilities GetCapabilities() const override;

    void Present() override;

    void Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil) override;

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

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) override;

    void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer) override;

    void SetSampler(int index, NativeSamplerState* sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets) override;

private:
    void BeginDraw();

private:
    id<MTLCommandQueue> commandQueue;
    id<MTLCommandBuffer> commandBuffer;
    id<MTLRenderCommandEncoder> commandEncoder;
    MTLPrimitiveType primitiveType;
    id<MTLBuffer> indexBuffer;
    MTLIndexType indexType;
    Optional<ClearCommandMetal> clearCommand;
    bool needToUpdateRenderPass;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
