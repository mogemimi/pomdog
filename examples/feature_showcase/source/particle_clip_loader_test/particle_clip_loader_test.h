#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/particles/particle_clip.h"
#include "pomdog/experimental/particles/particle_system.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class ParticleClipLoaderTest final : public Game {
public:
    explicit ParticleClipLoaderTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    initialize() override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    std::unique_ptr<ParticleSystem> particleSystem;
    Vector2 emitterPosition;
    std::size_t currentClipIndex = 0;
};

} // namespace feature_showcase
