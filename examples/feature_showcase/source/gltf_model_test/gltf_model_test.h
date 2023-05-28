#pragma once

#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class GLTFModelTest final : public Game {
public:
    explicit GLTFModelTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer1;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer2;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState1;
    std::shared_ptr<gpu::PipelineState> pipelineState2;
    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer;
    std::shared_ptr<gpu::Texture2D> texture;
};

} // namespace feature_showcase
