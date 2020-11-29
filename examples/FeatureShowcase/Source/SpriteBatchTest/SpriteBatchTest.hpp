#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class SpriteBatchTest final : public Game {
public:
    explicit SpriteBatchTest(const std::shared_ptr<GameHost>& gameHost);

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

    struct SpriteInstance final {
        Vector2 Position;
        Pomdog::Color Color;
        Vector2 Scale;
    };
    std::vector<SpriteInstance> sprites;

    std::shared_ptr<PrimitiveBatch> primitiveBatch;
};

} // namespace FeatureShowcase
