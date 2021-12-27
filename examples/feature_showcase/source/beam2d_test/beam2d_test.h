#pragma once

#include "pomdog/experimental/graphics/primitive_batch.hpp"
#include "pomdog/experimental/graphics/sprite_batch.hpp"
#include "pomdog/experimental/particles/beam_system.hpp"
#include "pomdog/pomdog.hpp"

namespace feature_showcase {

using namespace pomdog;

class Beam2DTest final : public Game {
public:
    explicit Beam2DTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    std::unique_ptr<BeamSystem> beamSystem;
    Vector2 emitterTarget;
};

} // namespace feature_showcase
