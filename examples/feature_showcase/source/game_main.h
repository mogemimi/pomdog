#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class Button {
public:
    std::string Text;
    Rect2D Rect;
    std::function<void()> OnClicked;
    bool Selected = false;

    Button() = default;
    Button(const std::string& text, std::function<void()> onClicked)
        : Text(text)
        , OnClicked(onClicked)
    {
    }
};

class GameMain final : public Game {
public:
    GameMain();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    void updateMenuLayout();

    void drawMenu();

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<GameClock> clock_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<Game> subGame_;

    std::shared_ptr<Timer> timer_;
    std::shared_ptr<Timer> fpsTimer_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<SpriteFont> spriteFont_;

    std::vector<Button> buttons_;
    std::vector<Button> hudButtons_;
    std::string footerString_;
    double scrollY_ = 0;
    i32 prevScrollWheel_ = 0;
    bool wasLeftMouseDown_ = false;
};

} // namespace feature_showcase
