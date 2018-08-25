// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "LightningTestGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include <utility>

namespace TestApp {

LightningTestGame::LightningTestGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
{}

LightningTestGame::~LightningTestGame() = default;

void LightningTestGame::Initialize()
{
    window->SetTitle("TestApp - Enjoy Game Dev, Have Fun.");
    window->SetAllowUserResizing(true);

    auto assets = gameHost->AssetManager();
    auto clientBounds = window->GetClientBounds();

    {
        gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
        editorBackground = std::make_unique<SceneEditor::EditorBackground>(gameHost);
    }
    {
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        //samplerPoint = SamplerState::CreatePointWrap(graphicsDevice);

        texture = assets->Load<Texture2D>("Particles/lightning.png");
    }
    {
        renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
            clientBounds.Width, clientBounds.Height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
    }
    {
        spriteBatch = std::make_unique<SpriteBatch>(graphicsDevice, *assets);
        spriteRenderer = std::make_unique<SpriteRenderer>(graphicsDevice, *assets);
        fxaa = std::make_unique<FXAA>(graphicsDevice, *assets);
        fxaa->SetViewport(clientBounds.Width, clientBounds.Height);
        screenQuad = std::make_unique<ScreenQuad>(graphicsDevice);
    }
    {
        mainCamera = gameWorld.CreateObject();
        mainCamera.AddComponent<Transform2D>();
        mainCamera.AddComponent<Camera2D>();
    }
//    {
//        auto gameObject = gameWorld.CreateObject();
//
//        auto & sprite = gameObject->AddComponent<Sprite>();
//        sprite.Origin = Vector2{0.5f, 0.5f};
//        sprite.Subrect = Rectangle(0, 0, texture->Width(), texture->Height());
//
//        gameObject->AddComponent<Transform2D>();
//        gameObject->AddComponent<CanvasItem>();
//
//        auto transform = gameObject->Component<Transform2D>();
//        transform->Position = {0, 0};
//        transform->Scale = {2, 2};
//
//        auto node = std::make_shared<HierarchyNode>(gameObject);
//        rootNode->AddChild(node);
//    }
//
//    for (int i = 0; i < 10; ++i)
//    {
//        auto gameObject = gameWorld.CreateObject();
//        gameObject->AddComponent<CanvasItem>();
//
//        auto & sprite = gameObject->AddComponent<Sprite>();
//        auto & transform = gameObject->AddComponent<Transform2D>();
//
//        transform.Position = {i * 64 * 2.0f, 0};
//        transform.Scale = {2.0f, 2.0f};
//        transform.Rotation = (0.5f * i) * Math::PiOver4<float>;
//        sprite.Origin = Vector2{0.5f, 0.5f};
//        sprite.Subrect = Rectangle(0, 0, texture->Width(), texture->Height());//Rectangle(0, 0, 16, 28);
//
//        auto node = std::make_shared<HierarchyNode>(gameObject);
//        rootNode->AddChild(node);
//    }

    touchPoint = {0, -300};

    {
        scenePanel = std::make_shared<UI::ScenePanel>(clientBounds.Width, clientBounds.Height);
        scenePanel->cameraObject = mainCamera;
        gameEditor->AddView(scenePanel);
    }
    {
        auto stackPanel = std::make_shared<UI::StackPanel>(124, 170);
        stackPanel->Transform(Matrix3x2::CreateTranslation(Vector2{5, 10}));
        gameEditor->AddView(stackPanel);

        {
            auto navigator = std::make_shared<UI::DebugNavigator>(gameHost->Clock());
            stackPanel->AddChild(navigator);
        }
        {
            slider1 = std::make_shared<UI::Slider>(0, 100);
            slider1->Value(34);
            stackPanel->AddChild(slider1);
        }
        {
            slider2 = std::make_shared<UI::Slider>(0.1, 4.0);
            slider2->Value(1.2);
            stackPanel->AddChild(slider2);
        }
        {
            slider3 = std::make_shared<UI::Slider>(0.0f, 1.0f);
            slider3->Value(0.2f);
            stackPanel->AddChild(slider3);
        }
        {
            slider4 = std::make_shared<UI::Slider>(0.0f, 70.0f);
            slider4->Value(8.0f);
            stackPanel->AddChild(slider4);
        }
        {
            slider5 = std::make_shared<UI::Slider>(0.0f, 70.0f);
            slider5->Value(8.0f);
            stackPanel->AddChild(slider5);
        }
    }

    clientViewport = Viewport{0, 0, clientBounds.Width, clientBounds.Height};

    connections.Connect(scenePanel->SceneTouch, [this](Vector2 const& positionInView) {
        auto transform = mainCamera.Component<Transform2D>();
        auto camera = mainCamera.Component<Camera2D>();

        POMDOG_ASSERT(transform && camera);
        auto inverseViewMatrix3D = Matrix4x4::Invert(SandboxHelper::CreateViewMatrix(*transform, *camera));

        auto position = Vector3::Transform(Vector3(
            positionInView.X - clientViewport.Width / 2,
            positionInView.Y - clientViewport.Height / 2,
            0), inverseViewMatrix3D);

        touchPoint = Vector2{position.X, position.Y};
    });

    connections.Connect(window->ClientSizeChanged, [this](int width, int height) {
        clientViewport = Viewport{0, 0, width, height};

        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice, width, height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

        fxaa->SetViewport(width, height);
        spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(width, height, 1.0f, 100.0f));
    });
}

void LightningTestGame::Update()
{
    auto clock = gameHost->Clock();
    {
        gameEditor->Update();
    }
    {
        beamSystem.emitter.InterpolationPoints = slider1->Value();
        beamSystem.emitter.StartThickness = slider2->Value();
        beamSystem.branching.BranchingRate = slider3->Value();
        beamSystem.emitter.SwayRange = std::uniform_real_distribution<float>(-slider4->Value(), slider4->Value());
        beamSystem.branching.SpreadRange = std::uniform_real_distribution<float>(-slider5->Value(), slider5->Value());
    }
    {
        Transform2D transform;
        transform.Scale = {1.0f, 1.0f};
        transform.Rotation = 0.0f;
        transform.Position = Vector2::Zero;
        beamSystem.Update(clock->FrameDuration(), transform, touchPoint);
    }
}

void LightningTestGame::DrawSprites()
{
    auto transform = mainCamera.Component<Transform2D>();
    auto camera = mainCamera.Component<Camera2D>();

    POMDOG_ASSERT(transform && camera);
    auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        clientViewport.Width, clientViewport.Height, 0.1f, 1000.0f);

    editorBackground->SetViewProjection(viewMatrix * projectionMatrix);

    // NOTE: Changing blend state
    //graphicsContext->SetBlendState(blendStateAdditive);

    POMDOG_ASSERT(spriteRenderer);
    spriteRenderer->Begin(SpriteSortMode::Deferred, viewMatrix);
    {
        SpriteLine spriteLine;
        spriteLine.Texture = texture;
        spriteLine.HalfCircleSize = {8, 32};
        spriteLine.InverseThickness = 5.0f;
        spriteLine.StartRectangle = {0, 0, 32, 64};
        spriteLine.MiddleRectangle = {32, 0, 1, 64};
        spriteLine.EndRectangle = {33, 0, 31, 64};

        auto DrawBeam = [&](std::vector<Vector2> const& points, float lineThickness, Color const& color)
        {
            for (std::size_t i = 1; i < points.size(); ++i)
            {
                POMDOG_ASSERT(i > 0);
                auto & start = points[i - 1];
                auto & end = points[i];
                spriteLine.Draw(*spriteRenderer, start, end, lineThickness, color, 0);
            }
        };

        for (auto & beam: beamSystem.beams)
        {
            DrawBeam(beam.Points, beam.Thickness, beam.Color);
        }
    }
    spriteRenderer->End();
}

void LightningTestGame::Draw()
{
    constexpr bool enableFxaa = true;

    if (enableFxaa) {
        auto bounds = renderTarget->Bounds();
        graphicsContext->SetRenderTarget(renderTarget);
        graphicsContext->SetViewport(Viewport{bounds});
        graphicsContext->SetScissorRectangle(bounds);
    }

    SceneEditor::EditorColorScheme colorScheme;

    graphicsContext->Clear(colorScheme.BackgroundDark);
    editorBackground->Draw(*graphicsContext);

    //graphicsContext->SetSamplerState(0, samplerPoint);
    DrawSprites();

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
