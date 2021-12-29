#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class GIFDecodeTest final : public Game {
public:
    explicit GIFDecodeTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    TexturePacker::TextureAtlas textureAtlas;
    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Timer> animationTimer;
    int currentFrameIndex;

    struct SpriteInstance final {
        Vector2 Position;
        pomdog::Color Color;
        Vector2 Scale;
        int StartFrameIndex;
    };
    std::vector<SpriteInstance> sprites;
};

} // namespace feature_showcase
