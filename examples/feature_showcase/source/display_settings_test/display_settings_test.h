// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/window_mode.h"
#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/gui/debug_navigator.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
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

    std::unique_ptr<gui::DrawingContext> drawingContext_;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy_;

    std::string statusText_;
};

} // namespace feature_showcase
