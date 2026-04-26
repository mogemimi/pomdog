#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class Texture2DLoaderTest final : public Game {
public:
    explicit Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<SpritePipeline> spritePipelineFont_;
    std::shared_ptr<SpriteFont> spriteFont_;

    std::shared_ptr<gpu::Texture2D> texturePNG_;
    std::shared_ptr<gpu::Texture2D> textureDXT1_;
    std::shared_ptr<gpu::Texture2D> textureDXT5_;
    std::shared_ptr<gpu::Texture2D> texturePNMP1_;
    std::shared_ptr<gpu::Texture2D> texturePNMP2_;
    std::shared_ptr<gpu::Texture2D> texturePNMP3_;
    std::shared_ptr<gpu::Texture2D> texturePNMP4_;
    std::shared_ptr<gpu::Texture2D> texturePNMP5_;
    std::shared_ptr<gpu::Texture2D> texturePNMP6_;
};

} // namespace feature_showcase
