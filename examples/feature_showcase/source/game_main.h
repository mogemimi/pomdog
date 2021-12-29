#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class Button {
public:
    std::string Text;
    Rectangle Rect;
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
    explicit GameMain(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    void UpdateMenuLayout();

    void DrawMenu();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<Game> subGame;

    std::shared_ptr<Timer> timer;
    std::shared_ptr<Timer> fpsTimer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;

    std::vector<Button> buttons;
    std::vector<Button> hudButtons;
    std::string footerString;
    double scrollY = 0;
};

} // namespace feature_showcase
