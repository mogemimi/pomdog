// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog.Experimental/InGameEditor/detail/EditorBackground.hpp>
#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SpriteRenderable;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class MaidBeamGame: public Game {
public:
    explicit MaidBeamGame(std::shared_ptr<GameHost> const& gameHost);

    ~MaidBeamGame();

    void Initialize();

    void Update();

    void Draw();

private:
    void DrawScene(Transform2D const& transform, Camera2D const& camera);

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;

    std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
    std::unique_ptr<SceneEditor::EditorBackground> editorBackground;
    bool sandboxMode;

    std::shared_ptr<UI::ScenePanel> scenePanel;
    std::shared_ptr<UI::Slider> slider1;
    std::shared_ptr<UI::Slider> slider2;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch1;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch2;
    std::shared_ptr<UI::TextBlock> textBlock1;

    GameWorld gameWorld;
    std::unique_ptr<GameLevel> level;
    GameObject editorCamera;

    Renderer renderer;
    std::shared_ptr<RenderTarget2D> renderTarget;
    std::unique_ptr<FXAA> fxaa;
    std::unique_ptr<ScreenQuad> screenQuad;
    std::vector<SpriteRenderable> cameraSprites;

    ConnectionList connections;
    Viewport clientViewport;
};

} // namespace TestApp
