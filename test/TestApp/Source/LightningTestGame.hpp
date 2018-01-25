// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog.Experimental/InGameEditor/detail/EditorBackground.hpp>
#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SpriteBatch;
class SpriteFont;
class SpriteRenderer;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class LightningTestGame: public Game {
public:
    explicit LightningTestGame(std::shared_ptr<GameHost> const& gameHost);

    ~LightningTestGame();

    void Initialize();

    void Update();

    void Draw();

private:
    void DrawSprites();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;

    std::shared_ptr<Texture2D> texture;
    std::unique_ptr<SpriteBatch> spriteBatch;
    std::unique_ptr<SpriteRenderer> spriteRenderer;

    //std::shared_ptr<SamplerState> samplerPoint;
    std::shared_ptr<RenderTarget2D> renderTarget;
    std::unique_ptr<FXAA> fxaa;
    std::unique_ptr<ScreenQuad> screenQuad;

    std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
    std::unique_ptr<SceneEditor::EditorBackground> editorBackground;
    std::shared_ptr<UI::ScenePanel> scenePanel;
    std::shared_ptr<UI::Slider> slider1;
    std::shared_ptr<UI::Slider> slider2;
    std::shared_ptr<UI::Slider> slider3;
    std::shared_ptr<UI::Slider> slider4;
    std::shared_ptr<UI::Slider> slider5;

    GameWorld gameWorld;
    GameObject mainCamera;

    BeamSystem beamSystem;
    Vector2 touchPoint;

    ConnectionList connections;
    Viewport clientViewport;
};

} // namespace TestApp
