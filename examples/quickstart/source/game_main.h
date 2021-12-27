#pragma once

#include "pomdog/pomdog.h"

namespace quickstart {

using namespace pomdog;

struct alignas(16) MyShaderConstants final {
    Matrix4x4 Model;
    Matrix4x4 ViewProjection;
};

class GameMain final : public Game {
public:
    explicit GameMain(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    std::unique_ptr<Timer> timer;
    MyShaderConstants myShaderConstants;
    ConnectionList connect;
};

} // namespace quickstart
