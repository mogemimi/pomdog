#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/experimental/texture_atlas/texture_atlas_dynamic.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class TextureAtlasDynamicTest final : public Game {
public:
    explicit TextureAtlasDynamicTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    // Dynamic atlas built at runtime from pomdog.png
    std::shared_ptr<TextureAtlasDynamic> dynamicAtlas_;
    // Individual sprite regions from the atlas
    std::vector<TextureRegion> spriteRegions_;

    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;

    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    std::unique_ptr<gui::DrawingContext> drawingContext_;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy_;

    int selectedRegion_ = 0;
    std::vector<std::string> regionNames_;
};

} // namespace feature_showcase
