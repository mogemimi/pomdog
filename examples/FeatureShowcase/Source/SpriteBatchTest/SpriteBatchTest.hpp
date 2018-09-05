#pragma once

#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Experimental/Graphics/PolygonBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/TexturePacker/TextureAtlas.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class SpriteBatchTest final : public Game {
public:
    explicit SpriteBatchTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    TexturePacker::TextureAtlas textureAtlas;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<Texture2D> texture2;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Timer> animationTimer;
    int currentFrameIndex;

    struct SpriteInstance final {
        Vector2 Position;
        Pomdog::Color Color;
        Vector2 Scale;
        int StartFrameIndex;
    };
    std::vector<SpriteInstance> sprites;

    std::shared_ptr<PolygonBatch> polygonBatch;
};

} // namespace FeatureShowcase
