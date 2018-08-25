// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "MaidBeamGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include "Levels/GunShootingLevel.hpp"
#include <utility>
#include <random>

namespace TestApp {

MaidBeamGame::MaidBeamGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
    , sandboxMode(false)
{}

MaidBeamGame::~MaidBeamGame() = default;

void MaidBeamGame::Initialize()
{
    window->SetTitle("MaidBeamGame - Enjoy Game Dev, Have Fun.");
    window->SetAllowUserResizing(true);

    auto assets = gameHost->AssetManager();
    auto clientBounds = window->GetClientBounds();

    {
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
            clientBounds.Width, clientBounds.Height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
    }
    {
        fxaa = std::make_unique<FXAA>(graphicsDevice, *assets);
        fxaa->SetViewport(clientBounds.Width, clientBounds.Height);
        screenQuad = std::make_unique<ScreenQuad>(graphicsDevice);
    }
    {
        editorCamera = gameWorld.CreateObject();
        editorCamera.AddComponent<Transform2D>();
        editorCamera.AddComponent<Camera2D>();

        level = std::make_unique<GunShootingLevel>(*gameHost, gameWorld);
    }
    {
        auto texture = assets->Load<Texture2D>("EditorResources/CameraIcon.png");
        for (size_t i = 0; i < 10; ++i) {
            cameraSprites.emplace_back(texture);
            cameraSprites.back().DrawOrder = 10000.0f;
        }
    }
    {
        gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
        editorBackground = std::make_unique<SceneEditor::EditorBackground>(gameHost);
    }
    {
        scenePanel = std::make_shared<UI::ScenePanel>(clientBounds.Width, clientBounds.Height);
        scenePanel->cameraObject = editorCamera;
        gameEditor->AddView(scenePanel);
    }
    {
        auto stackPanel = std::make_shared<UI::StackPanel>(140, 170);
        stackPanel->Transform(Matrix3x2::CreateTranslation(Vector2{5, 10}));
        gameEditor->AddView(stackPanel);

        {
            auto navigator = std::make_shared<UI::DebugNavigator>(gameHost->Clock());
            stackPanel->AddChild(navigator);
        }
        {
            textBlock1 = std::make_shared<UI::TextBlock>();
            textBlock1->Text("Draw Calls: --");
            stackPanel->AddChild(textBlock1);
        }
        {
            slider1 = std::make_shared<UI::Slider>(0.3, 1.6);
            slider1->Value(1.48);
            stackPanel->AddChild(slider1);
        }
        {
            slider2 = std::make_shared<UI::Slider>(0.0, 1.0);
            slider2->Value(1.0);
            stackPanel->AddChild(slider2);
        }
        {
            toggleSwitch1 = std::make_shared<UI::ToggleSwitch>();
            toggleSwitch1->IsOn(true);
            toggleSwitch1->OnContent("Play");
            toggleSwitch1->OffContent("Stop");
            stackPanel->AddChild(toggleSwitch1);
        }
        {
            toggleSwitch2 = std::make_shared<UI::ToggleSwitch>();
            toggleSwitch2->IsOn(sandboxMode);
            toggleSwitch2->OnContent("Sandbox On");
            toggleSwitch2->OffContent("Sandbox Off");
            connections.Connect(toggleSwitch2->Toggled, [this](bool isOn) {
                sandboxMode = isOn;
                scenePanel->IsEnabled(sandboxMode);
            });

            stackPanel->AddChild(toggleSwitch2);
        }
    }

    clientViewport = Viewport{0, 0, clientBounds.Width, clientBounds.Height};

    connections.Connect(window->ClientSizeChanged, [this](int width, int height) {
        clientViewport = Viewport{0, 0, width, height};

        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice, width, height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

        fxaa->SetViewport(width, height);
        Log::Info("ClientSizeChanged!");
    });
}

void MaidBeamGame::Update()
{
    auto clock = gameHost->Clock();

    level->Update(*gameHost, gameWorld);

    for (auto & gameObject: gameWorld.QueryComponents<Animator>())
    {
        auto animator = gameObject.Component<Animator>();

        POMDOG_ASSERT(animator);
        animator->Update(clock->FrameDuration());
    }

    for (auto & gameObject: gameWorld.QueryComponents<Behavior>())
    {
        auto behavior = gameObject.Component<Behavior>();

        POMDOG_ASSERT(behavior);
        behavior->Update(gameObject, clock->FrameDuration());
    }

    gameWorld.Refresh();

    {
        gameEditor->Update();
        textBlock1->Text(StringHelper::Format("Draw Calls: %d", renderer.DrawCallCount()));
    }
}

void MaidBeamGame::DrawScene(Transform2D const& transform, Camera2D const& camera)
{
    auto clientBounds = window->GetClientBounds();

    Viewport viewport(
        clientBounds.Width * camera.NormalizedViewportX,
        clientBounds.Height * camera.NormalizedViewportY,
        clientBounds.Width * camera.NormalizedViewportWidth,
        clientBounds.Height * camera.NormalizedViewportHeight);

    auto viewMatrix = SandboxHelper::CreateViewMatrix(transform, camera);
    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        viewport.Width, viewport.Height, camera.Near, camera.Far);

    editorBackground->SetViewProjection(viewMatrix * projectionMatrix);
    renderer.ViewMatrix(viewMatrix);
    renderer.ProjectionMatrix(projectionMatrix);

    for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
    {
        auto renderable = gameObject.Component<Renderable>();
        renderable->Visit(gameObject, renderer);
    }

    graphicsContext->SetViewport(viewport);

    if (sandboxMode)
    {
        size_t cameraIndex = 0;
        for (auto & gameObject: gameWorld.QueryComponents<Camera2D, Transform2D>())
        {
            if (cameraIndex >= cameraSprites.size()) {
                break;
            }
            if (editorCamera == gameObject) {
                continue;
            }
            cameraSprites[cameraIndex].Visit(gameObject, renderer);
            ++cameraIndex;
        }

        editorBackground->Draw(*graphicsContext);
    }

    renderer.Render(*graphicsContext);
}

void MaidBeamGame::Draw()
{
    constexpr bool enableFxaa = true;

    if (enableFxaa) {
        auto bounds = renderTarget->Bounds();
        graphicsContext->SetRenderTarget(renderTarget);
        graphicsContext->SetViewport(Viewport{bounds});
        graphicsContext->SetScissorRectangle(bounds);
    }

    {
        auto & viewport = clientViewport;

        if (!sandboxMode)
        {
//            bool cleared = false;

            for (auto & cameraObject: gameWorld.QueryComponents<Camera2D, Transform2D>())
            {
                auto camera = cameraObject.Component<Camera2D>();

//                if (!camera->Enabled) {
//                    continue;
//                }
//
//                if (!cleared)
//                {
//                    graphicsContext->Clear(camera->BackgroundColor);
//                    cleared = true;
//                }

                auto transform = cameraObject.Component<Transform2D>();

                POMDOG_ASSERT(transform && camera);
                DrawScene(*transform, *camera);
            }
        }
        else
        {
            auto camera = editorCamera.Component<Camera2D>();
            auto transform = editorCamera.Component<Transform2D>();

//            graphicsContext->Clear(camera->BackgroundColor);
            DrawScene(*transform, *camera);
        }

        graphicsContext->SetViewport(viewport);
    }

    if (enableFxaa) {
        graphicsContext->SetRenderTarget();
        graphicsContext->SetViewport(clientViewport);
        graphicsContext->SetScissorRectangle(clientViewport.GetBounds());
        graphicsContext->Clear(Color::CornflowerBlue);
        fxaa->SetTexture(renderTarget);
        fxaa->Apply(*graphicsContext);
        screenQuad->DrawQuad(*graphicsContext);
    }

    gameEditor->DrawGUI(*graphicsContext);
    commandQueue->Present();
}

} // namespace TestApp
