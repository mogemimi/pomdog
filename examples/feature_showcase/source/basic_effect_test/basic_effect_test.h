#pragma once

#include "pomdog/pomdog.hpp"

namespace feature_showcase {

using namespace pomdog;

class BasicEffectTest final : public Game {
public:
    explicit BasicEffectTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<VertexBuffer> vertexBuffer1;
    std::shared_ptr<VertexBuffer> vertexBuffer2;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState1;
    std::shared_ptr<PipelineState> pipelineState2;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<ConstantBuffer> worldConstantBuffer;
    std::shared_ptr<Texture2D> texture;
};

} // namespace feature_showcase
