#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Particles/ParticleClip.hpp>
#include <Pomdog/Experimental/Particles/ParticleSystem.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class ParticleClipLoaderTest final : public Game {
public:
    explicit ParticleClipLoaderTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::shared_ptr<Error>
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

    std::unique_ptr<ParticleSystem> particleSystem;
    Vector2 emitterPosition;
    std::size_t currentClipIndex = 0;
};

} // namespace FeatureShowcase
