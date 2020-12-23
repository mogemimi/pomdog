#pragma once

#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class MultiRenderTargetTest final : public Game {
public:
    explicit MultiRenderTargetTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<RenderTarget2D> renderTargetAlbedo;
    std::shared_ptr<RenderTarget2D> renderTargetNormal;
    std::shared_ptr<RenderTarget2D> renderTargetDepth;
    std::shared_ptr<RenderTarget2D> renderTargetLighting;
    std::shared_ptr<DepthStencilBuffer> depthStencilBuffer;

    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<ConstantBuffer> worldConstantBuffer;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
};

} // namespace FeatureShowcase
