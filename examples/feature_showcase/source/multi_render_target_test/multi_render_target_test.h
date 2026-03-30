#pragma once

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class MultiRenderTargetTest final : public Game {
public:
    explicit MultiRenderTargetTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<vfs::FileSystemContext> fs_;
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
    std::shared_ptr<SpritePipeline> spritePipeline;
};

} // namespace feature_showcase
