// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <memory>
#include <vector>

namespace Pomdog::Detail::Metal {

class GraphicsContextMetal final : public NativeGraphicsContext {
public:
    GraphicsContextMetal() = delete;

    explicit GraphicsContextMetal(id<MTLDevice> nativeDevice);

    ~GraphicsContextMetal();

    GraphicsCapabilities GetCapabilities() const override;

    void ExecuteCommandLists(
        const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists) override;

    void Present() override;

    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    void SetRenderPass(const RenderPass& renderPass) override;

    void SetScissorRect(const Rectangle& scissorRect) override;

    void SetBlendFactor(const Vector4& blendFactor) override;

    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) override;

    void SetConstantBuffer(
        int index,
        const std::shared_ptr<NativeBuffer>& constantBuffer,
        std::size_t offset,
        std::size_t sizeInBytes) override;

    void SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) override;

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    void DispatchSemaphoreWait();

    void SetMTKView(MTKView* view);

private:
#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets;
#endif
    dispatch_semaphore_t inflightSemaphore;
    id<MTLCommandQueue> commandQueue;
    id<MTLCommandBuffer> commandBuffer;
    id<MTLRenderCommandEncoder> commandEncoder;
    MTLPrimitiveType primitiveType;
    id<MTLBuffer> indexBuffer;
    MTLIndexType indexType;
    MTKView* targetView;
    bool isDrawing = false;
};

} // namespace Pomdog::Detail::Metal
