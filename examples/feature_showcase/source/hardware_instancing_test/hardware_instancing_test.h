#pragma once

#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class HardwareInstancingTest final : public Game {
public:
    explicit HardwareInstancingTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::VertexBuffer> instanceBuffer_;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;

    struct alignas(16) SpriteInfo final {
        // {xy__} = position.xy
        // {__zw} = scale.xy
        Vector4 Translation;

        // {rgb_} = color.rgb
        // {___a} = color.a
        Vector4 Color;
    };

    std::vector<SpriteInfo> sprites_;
};

} // namespace feature_showcase
