#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class SpriteBatchTest final : public Game {
public:
    explicit SpriteBatchTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

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
    std::shared_ptr<SpriteBatch> spriteBatchNoOptimization_;
    std::shared_ptr<SpriteBatch> spriteBatchSortedSingleTexture_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::optional<SpriteSortMode> sortMode_ = std::nullopt;
    SpriteBatchOptimizationKind optimizationKind_ = SpriteBatchOptimizationKind::NotSpecified;
    Vector2 originPivot_ = Vector2::createZero();
    std::shared_ptr<Timer> timer_;

    struct SpriteInstance final {
        Vector2 position = {};
        Vector2 scale = {};
        Color color = Color::createWhite();
    };
    std::vector<SpriteInstance> sprites_;

    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    std::unique_ptr<gui::DrawingContext> drawingContext_;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy_;
};

} // namespace feature_showcase
