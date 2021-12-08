#pragma once

#include <pomdog/experimental/graphics/primitive_batch.hpp>
#include <pomdog/experimental/graphics/sprite_batch.hpp>
#include <pomdog/experimental/graphics/sprite_font.hpp>
#include <pomdog/experimental/graphics/truetype_font.hpp>
#include <pomdog/experimental/graphics/truetype_font_loader.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace pomdog;

class DistanceFieldFontTest final : public Game {
public:
    explicit DistanceFieldFontTest(const std::shared_ptr<GameHost>& gameHost);

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
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
};

} // namespace FeatureShowcase
