#pragma once

#include "pomdog/experimental/graphics/billboard_effect.h"
#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/experimental/particles/particle_clip.h"
#include "pomdog/experimental/particles/particle_system.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

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
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<BillboardBatchBuffer> billboardBuffer;
    std::shared_ptr<BillboardBatchEffect> billboardEffect;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<LineBatch> lineBatch;

    std::unique_ptr<ParticleSystem> particleSystem;
    Vector3 emitterPosition;
    std::size_t currentClipIndex = 0;
};

} // namespace feature_showcase
