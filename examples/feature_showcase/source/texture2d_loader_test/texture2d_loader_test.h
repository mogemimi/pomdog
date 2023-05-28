#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class Texture2DLoaderTest final : public Game {
public:
    explicit Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHost);

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
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;

    std::shared_ptr<gpu::Texture2D> texturePNG;
    std::shared_ptr<gpu::Texture2D> textureDXT1;
    std::shared_ptr<gpu::Texture2D> textureDXT5;
    std::shared_ptr<gpu::Texture2D> texturePNMP1;
    std::shared_ptr<gpu::Texture2D> texturePNMP2;
    std::shared_ptr<gpu::Texture2D> texturePNMP3;
    std::shared_ptr<gpu::Texture2D> texturePNMP4;
    std::shared_ptr<gpu::Texture2D> texturePNMP5;
    std::shared_ptr<gpu::Texture2D> texturePNMP6;
};

} // namespace feature_showcase
