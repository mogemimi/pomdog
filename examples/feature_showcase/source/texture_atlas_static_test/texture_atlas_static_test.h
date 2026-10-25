#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/texture_region.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// TextureAtlasStaticTest demonstrates loading pre-built `.tileset` files
/// (produced by png2atlas or piskel2atlas) and drawing their sprites with
/// SpriteBatch. The panel picks a sprite, plays its frames, and shows where
/// the current frame lies in the atlas texture.
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
    /// Holds the frames of a sprite that are named name_1, name_2, ... in its
    /// atlas, or the single region named name for a still sprite.
    struct Sprite final {
        std::string_view name;
        std::string_view texturePath;
        std::shared_ptr<TextureAtlas> atlas;
        std::vector<TextureRegion> frames;
        f32 framesPerSecond = 0.0f;
    };

    void buildUI(devgui::UI& ui);

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;

    std::vector<Sprite> sprites_;
    std::vector<std::string_view> spriteNames_;

    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    DevGUIOverlay overlay_;

    i32 selectedSprite_ = 0;
    i32 frameIndex_ = 0;
    f64 frameTime_ = 0.0;
    f32 framesPerSecond_ = 12.0f;
    f32 scale_ = 3.0f;
    bool playing_ = true;
    bool showAtlas_ = true;
};

} // namespace feature_showcase
