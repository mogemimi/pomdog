#pragma once

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class SpriteBatchEffectTest final : public Game {
public:
    explicit SpriteBatchEffectTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    std::shared_ptr<gpu::Texture2D> texture_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipelineSprite_;
    std::shared_ptr<SpritePipeline> spritePipelineSolidFill_;
    std::shared_ptr<SpritePipeline> spritePipelineWaterLine_;
    std::shared_ptr<Timer> timer_;
};

} // namespace feature_showcase
