#pragma once

#include <pomdog/experimental/graphics/billboard_effect.hpp>
#include <pomdog/experimental/graphics/line_batch.hpp>
#include <pomdog/experimental/particles/particle_clip.hpp>
#include <pomdog/experimental/particles/particle_system.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace pomdog;

class Particle3DTest final : public Game {
public:
    explicit Particle3DTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<BillboardBatchBuffer> billboardBuffer;
    std::shared_ptr<BillboardBatchEffect> billboardEffect;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<LineBatch> lineBatch;

    std::unique_ptr<ParticleSystem> particleSystem;
    Vector3 emitterPosition;
    std::size_t currentClipIndex = 0;
};

} // namespace FeatureShowcase
