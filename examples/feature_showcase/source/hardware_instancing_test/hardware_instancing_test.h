#pragma once

#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class HardwareInstancingTest final : public Game {
public:
    explicit HardwareInstancingTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::VertexBuffer> instanceBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    std::shared_ptr<gpu::Texture2D> texture;

    struct alignas(16) SpriteInfo final {
        // {xy__} = position.xy
        // {__zw} = scale.xy
        Vector4 Translation;

        // {rgb_} = color.rgb
        // {___a} = color.a
        Vector4 Color;
    };

    std::vector<SpriteInfo> sprites;
};

} // namespace feature_showcase
