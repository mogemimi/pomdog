#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class Texture2DLoaderTest final : public Game {
public:
    explicit Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHost);

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

} // namespace FeatureShowcase
