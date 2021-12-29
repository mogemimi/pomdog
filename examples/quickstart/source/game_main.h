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
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    std::unique_ptr<Timer> timer;
    MyShaderConstants myShaderConstants;
    ConnectionList connect;
};

} // namespace quickstart
