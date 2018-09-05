#pragma once

#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/PolygonBatch.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

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

class FeatureShowcaseGame final : public Game {
public:
    explicit FeatureShowcaseGame(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    void UpdateMenuLayout();

    void DrawMenu();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<Game> subGame;

    std::shared_ptr<Timer> timer;
    std::shared_ptr<Timer> fpsTimer;
    std::shared_ptr<PolygonBatch> polygonBatch;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;

    std::vector<Button> buttons;
    std::vector<Button> hudButtons;
    std::string footerString;
};

} // namespace FeatureShowcase
