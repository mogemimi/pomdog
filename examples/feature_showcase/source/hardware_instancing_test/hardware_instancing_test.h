#pragma once

#include "pomdog/pomdog.hpp"

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
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<VertexBuffer> instanceBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::shared_ptr<Texture2D> texture;

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
