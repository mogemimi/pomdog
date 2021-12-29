#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class SpriteLineTest final : public Game {
public:
    explicit SpriteLineTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;

    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    Vector2 mousePosition;
};

} // namespace feature_showcase
