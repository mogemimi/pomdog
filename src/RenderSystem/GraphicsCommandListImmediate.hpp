// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "NativeGraphicsCommandList.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class NativeGraphicsContext;

enum class GraphicsCommandType : std::int8_t {
    DrawCommand,
    DrawIndexedCommand,
    DrawInstancedCommand,
    DrawIndexedInstancedCommand,
    SetPrimitiveTopologyCommand,
    SetBlendFactorCommand,
    SetVertexBuffersCommand,
    SetIndexBufferCommand,
    SetPipelineStateCommand,
    SetConstantBufferCommand,
    SetSamplerStateCommand,
    SetTextureCommand,
    SetTextureRenderTarget2DCommand,
    SetRenderPassCommand,
};

class GraphicsCommand {
public:
    virtual ~GraphicsCommand() = default;

    virtual void Execute(NativeGraphicsContext & graphicsContext) = 0;

    GraphicsCommandType commandType;
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

    void SetRenderPass(RenderPass && renderPass) override;

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

    void ExecuteImmediate(NativeGraphicsContext & graphicsContext);

private:
    void SortCommandsForMetal();

private:
    std::vector<std::shared_ptr<Detail::GraphicsCommand>> commands;
};

} // namespace Detail
} // namespace Pomdog
