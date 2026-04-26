#pragma once

#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class GLTFModelTest final : public Game {
public:
    explicit GLTFModelTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer1_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer2_;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState1_;
    std::shared_ptr<gpu::PipelineState> pipelineState2_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;
};

} // namespace feature_showcase
