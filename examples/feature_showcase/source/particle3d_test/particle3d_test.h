#pragma once

#include "pomdog/experimental/graphics/billboard_effect.h"
#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/experimental/particles/particle_clip.h"
#include "pomdog/experimental/particles/particle_system.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class Particle3DTest final : public Game {
public:
    explicit Particle3DTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<BillboardBatchBuffer> billboardBuffer_;
    std::shared_ptr<BillboardBatchEffect> billboardEffect_;
    std::shared_ptr<Timer> timer_;
    std::shared_ptr<LineBatch> lineBatch_;
    std::shared_ptr<LinePipeline> linePipeline_;

    std::unique_ptr<ParticleSystem> particleSystem_;
    Vector3 emitterPosition_;
    std::size_t currentClipIndex_ = 0;
    bool wasRightMouseDown_ = false;
};

} // namespace feature_showcase
