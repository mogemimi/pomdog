#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/sprite_font_sdf.h"
#include "pomdog/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class DistanceFieldFontTest final : public Game {
public:
    explicit DistanceFieldFontTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    void buildUI(devgui::UI& ui);

    /// Applies the SDF parameters and outline of the preset at index.
    void applyPreset(i32 index) noexcept;

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;

    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipelineDF_;
    std::shared_ptr<SpritePipeline> spritePipelineDFOutline_;
    std::shared_ptr<SpriteFont> spriteFont_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    DevGUIOverlay overlay_;

    f32 fontSmoothing_ = 0.163f;
    f32 fontWeight_ = 0.813f;
    f32 outlineWeight_ = 0.440f;
    f32 fontScale_ = 1.0f;
    Color outlineColor_ = Color::createBlack();
    bool useAutomaticSDFParameters_ = true;
    bool useOutline_ = true;
    i32 presetIndex_ = 1;
};

} // namespace feature_showcase
