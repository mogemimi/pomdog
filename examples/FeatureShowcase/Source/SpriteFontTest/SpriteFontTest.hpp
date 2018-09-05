#pragma once

#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Experimental/Graphics/PolygonBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class SpriteFontTest final : public Game {
public:
    explicit SpriteFontTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::shared_ptr<PolygonBatch> polygonBatch;
};

} // namespace FeatureShowcase
