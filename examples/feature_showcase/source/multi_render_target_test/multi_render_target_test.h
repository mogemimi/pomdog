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
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<gpu::RenderTarget2D> renderTargetAlbedo_;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetNormal_;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetDepth_;
    std::shared_ptr<gpu::RenderTarget2D> renderTargetLighting_;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer_;

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
};

} // namespace feature_showcase
