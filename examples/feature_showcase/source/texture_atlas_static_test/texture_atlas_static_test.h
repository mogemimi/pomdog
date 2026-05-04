#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/experimental/texture_atlas/texture_region.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// TextureAtlasStaticTest demonstrates loading pre-built `.tileset` files
/// (produced by png2atlas or piskel2atlas) and drawing animated and static
/// sprites using SpriteBatch.
class TextureAtlasStaticTest final : public Game {
public:
    explicit TextureAtlasStaticTest(
        const std::shared_ptr<GameHost>& gameHost,
        const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    std::shared_ptr<TextureAtlas> atlasGirlWithGun_;
    std::shared_ptr<TextureAtlas> atlasMisc_;
    std::shared_ptr<TextureAtlas> atlasSpritesheet_;

    std::vector<TextureRegion> girlWithGunFrames_;
    std::vector<TextureRegion> gothapronFrames_;
    std::vector<TextureRegion> coinFrames_;
    std::vector<TextureRegion> punchMaidFrames_;
    std::vector<TextureRegion> sailorWalkFrames_;
    TextureRegion particleLightningRegion_;
    TextureRegion particleSmokeRegion_;
    TextureRegion pomdogRegion_;

    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    std::unique_ptr<gui::DrawingContext> drawingContext_;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy_;
};

} // namespace feature_showcase
