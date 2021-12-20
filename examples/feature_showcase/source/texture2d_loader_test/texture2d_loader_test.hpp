#pragma once

#include <pomdog/experimental/graphics/primitive_batch.hpp>
#include <pomdog/experimental/graphics/sprite_batch.hpp>
#include <pomdog/experimental/graphics/sprite_font.hpp>
#include <pomdog/experimental/graphics/truetype_font.hpp>
#include <pomdog/experimental/graphics/truetype_font_loader.hpp>
#include <pomdog/pomdog.hpp>

namespace feature_showcase {

using namespace pomdog;

class Texture2DLoaderTest final : public Game {
public:
    explicit Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHost);

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
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;

    std::shared_ptr<Texture2D> texturePNG;
    std::shared_ptr<Texture2D> textureDXT1;
    std::shared_ptr<Texture2D> textureDXT5;
    std::shared_ptr<Texture2D> texturePNMP1;
    std::shared_ptr<Texture2D> texturePNMP2;
    std::shared_ptr<Texture2D> texturePNMP3;
    std::shared_ptr<Texture2D> texturePNMP4;
    std::shared_ptr<Texture2D> texturePNMP5;
    std::shared_ptr<Texture2D> texturePNMP6;
};

} // namespace feature_showcase
