// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ParticleTestGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include <utility>

namespace TestApp {

ParticleTestGame::ParticleTestGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
{}

ParticleTestGame::~ParticleTestGame() = default;

void ParticleTestGame::Initialize()
{
    window->SetTitle("ParticleTestGame - Enjoy Game Dev, Have Fun.");
    window->SetAllowUserResizing(false);

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
        gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
        editorBackground = std::make_unique<SceneEditor::EditorBackground>(gameHost);
    }
    {
        mainCamera = gameWorld.CreateObject();
        mainCamera.AddComponent<Transform2D>();
        mainCamera.AddComponent<Camera2D>();
    }
    {
        particleObject = gameWorld.CreateObject();
        particleObject.AddComponent<Transform2D>();
        auto texture = assets->Load<Texture2D>("Particles/smoke.png");
        particleObject.AddComponent(std::make_unique<ParticleRenderable>(texture));
    }
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
            slider1 = std::make_shared<UI::Slider>(1, 512);
            slider1->Value(170.0);
            stackPanel->AddChild(slider1);
        }
        {
            slider2 = std::make_shared<UI::Slider>(-200.0, 200.0);
            slider2->Value(0.0);
            stackPanel->AddChild(slider2);
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
}

void ParticleTestGame::Update()
{
    auto clock = gameHost->Clock();
    {
        gameEditor->Update();
    }

    if (auto transform = particleObject.Component<Transform2D>())
    {
        transform->Position = touchPoint;
        transform->Rotation = Math::PiOver2<float>;
        transform->Scale = {1, 1};
    }

//    if (auto particleRenderable = particleObject.Component<ParticleRenderable>())
//    {
//        auto & particleSystem = particleRenderable->particleSystem;
//        particleSystem.emitter.EmissionRate = static_cast<std::uint16_t>(slider1->Value());
//        particleSystem.emitter.GravityModifier = slider2->Value();
//    }

//    for (auto & gameObject: gameWorld.QueryComponents<ParticleRenderable, Transform2D>())
//    {
//        auto particleRenderable = gameObject->Component<ParticleRenderable>();
//        particleRenderable->Update(*particleObject, *clock);
//    }

//    if (auto particleRenderable = particleObject.Component<ParticleRenderable>())
//    {
//        particleRenderable->Update(particleObject, *clock);
//    }
}

void ParticleTestGame::Draw()
{
    {
        auto transform = mainCamera.Component<Transform2D>();
        auto camera = mainCamera.Component<Camera2D>();

        POMDOG_ASSERT(transform && camera);
        auto clientBounds = window->GetClientBounds();
        auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
        auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
            clientBounds.Width, clientBounds.Height, camera->Near, camera->Far);

        editorBackground->SetViewProjection(viewMatrix * projectionMatrix);
    }

    for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
    {
        auto renderable = gameObject.Component<Renderable>();
        renderable->Visit(gameObject, renderer);
    }

    constexpr bool enableFxaa = true;

    if (enableFxaa) {
        auto bounds = renderTarget->Bounds();
        graphicsContext->SetRenderTarget(renderTarget);
        graphicsContext->SetViewport(Viewport{bounds});
        graphicsContext->SetScissorRectangle(bounds);
    }

    graphicsContext->Clear(Color::CornflowerBlue);
    editorBackground->Draw(*graphicsContext);
    renderer.Render(*graphicsContext);

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
