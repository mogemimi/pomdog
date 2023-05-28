#pragma once

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class MultiRenderTargetTest final : public Game {
public:
    explicit MultiRenderTargetTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    initialize() override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::RenderTarget2D> renderTargetAlbedo;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetNormal;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetDepth;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetLighting;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer;

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer;
    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
};

} // namespace feature_showcase
