#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class SVGDecodeTest final : public Game {
public:
    explicit SVGDecodeTest(const std::shared_ptr<GameHost>& gameHost);

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
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;

    std::vector<std::shared_ptr<Texture2D>> textures;
    std::vector<std::string> svgFiles;
};

} // namespace FeatureShowcase
