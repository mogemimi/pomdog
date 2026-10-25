// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/sprite_font_sdf.h"
#include "pomdog/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class DisplaySettingsTest final : public Game {
public:
    explicit DisplaySettingsTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    void buildUI(devgui::UI& ui);

    void applyWindowMode(WindowMode mode) noexcept;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<SpriteFont> spriteFont_;

    DevGUIOverlay overlay_;

    std::string statusText_;
    DisplayMetrics displayMetrics_;
    u32 displayMetricsChangeCount_ = 0;
    i32 frameRateCapIndex_ = 0;
    bool allowUserResizing_ = false;
    bool vsync_ = true;
};

} // namespace feature_showcase
