// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog.Experimental/InGameEditor/detail/EditorBackground.hpp>
#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class ParticleTestGame: public Game {
public:
    explicit ParticleTestGame(std::shared_ptr<GameHost> const& gameHost);

    ~ParticleTestGame();

    void Initialize();

    void Update();

    void Draw();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;

    std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
    std::unique_ptr<SceneEditor::EditorBackground> editorBackground;
    std::shared_ptr<UI::ScenePanel> scenePanel;
    std::shared_ptr<UI::Slider> slider1;
    std::shared_ptr<UI::Slider> slider2;

    GameWorld gameWorld;
    GameObject mainCamera;
    GameObject particleObject;

    Renderer renderer;
    std::shared_ptr<RenderTarget2D> renderTarget;
    std::unique_ptr<FXAA> fxaa;
    std::unique_ptr<ScreenQuad> screenQuad;

    ConnectionList connections;
    Viewport clientViewport;
    Vector2 touchPoint;
};

} // namespace TestApp
