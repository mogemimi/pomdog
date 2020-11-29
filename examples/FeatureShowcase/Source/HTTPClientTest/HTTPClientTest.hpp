#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class HTTPClientTest final : public Game {
public:
    explicit HTTPClientTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::string requestURL;
    std::string webText;
};

} // namespace FeatureShowcase
