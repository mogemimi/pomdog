#pragma once

#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace quickstart {

using namespace pomdog;

struct alignas(16) MyShaderConstants final {
    Matrix4x4 Model;
    Matrix4x4 ViewProjection;
};

class GameMain final : public Game {
public:
    GameMain();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<GameClock> clock_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::unique_ptr<Timer> timer_;
    MyShaderConstants myShaderConstants_;
    ConnectionList connect_;
};

} // namespace quickstart
