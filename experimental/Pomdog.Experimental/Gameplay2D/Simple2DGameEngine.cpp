// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Simple2DGameEngine.hpp"
#include "Pomdog.Experimental/Gameplay2D/ActorComponent.hpp"
#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/SpriteBatchCommandProcessor.hpp"

namespace Pomdog {

Simple2DGameEngine::Simple2DGameEngine(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , postProcessCompositor(gameHost->GetGraphicsDevice())
    , needToUpdateViewProjectionMatrix(true)
{
    auto graphicsDevice = gameHost->GetGraphicsDevice();
    auto assets = gameHost->GetAssetManager();
    auto window = gameHost->GetWindow();
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    postProcessCompositor.SetViewportSize(
        *graphicsDevice,
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        presentationParameters.DepthStencilFormat);

    {
        renderer = std::make_unique<Renderer>(graphicsDevice);

        renderer->AddProcessor(std::make_unique<
            Rendering::PrimitiveCommandProcessor>(graphicsDevice, *assets));
        renderer->AddProcessor(std::make_unique<
            Rendering::PrimitivePolygonCommandProcessor>(graphicsDevice, *assets));
        renderer->AddProcessor(std::make_unique<
            Rendering::SpriteBatchCommandProcessor>(graphicsDevice, *assets));
    }
    {
        commandListPreRender = std::make_shared<GraphicsCommandList>(*graphicsDevice);
        commandListPostRender = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        connections(window->ClientSizeChanged, [this](int width, int height) {
            this->OnViewportSizeChanged(width, height);
        });

        auto bounds = window->GetClientBounds();
        viewport = {0, 0, bounds.Width, bounds.Height};
        OnViewportSizeChanged(bounds.Width, bounds.Height);
    }
}

Simple2DGameEngine::~Simple2DGameEngine()
{
    entityManager.Clear();
}

void Simple2DGameEngine::OnViewportSizeChanged(int width, int height)
{
    auto graphicsDevice = gameHost->GetGraphicsDevice();
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    viewport.Width = width;
    viewport.Height = height;

    renderTarget = std::make_shared<RenderTarget2D>(
        graphicsDevice,
        width,
        height,
        false,
        presentationParameters.BackBufferFormat,
        presentationParameters.DepthStencilFormat);

    postProcessCompositor.SetViewportSize(
        *graphicsDevice,
        width,
        height,
        presentationParameters.DepthStencilFormat);

    if (mainCamera) {
        mainCamera->SetViewportSize(width, height);
    }

    needToUpdateViewProjectionMatrix = true;
}

void Simple2DGameEngine::Update()
{
    auto clock = gameHost->GetClock();
    auto frameDuration = clock->GetFrameDuration();

    for (auto & entity : entityManager.QueryComponents<ActorComponent>()) {
        auto actor = entity.GetComponent<ActorComponent>();
        actor->Act(entity, frameDuration);
    }

    entityManager.Refresh();

    // TODO: bad code
    needToUpdateViewProjectionMatrix = true;
}

void Simple2DGameEngine::Draw()
{
    if (needToUpdateViewProjectionMatrix) {
        auto viewMatrix = [&]() -> Matrix4x4 {
            if (mainCamera && mainCameraTransform) {
                auto rotation = Matrix4x4::CreateFromQuaternion(mainCameraTransform->GetRotation());
                return Matrix4x4::CreateLookAtLH(
                    mainCameraTransform->GetPosition(),
                    mainCameraTransform->GetPosition() + Vector3::Transform(Vector3::UnitZ, rotation),
                    Vector3::UnitY);
            }
            return Matrix4x4::Identity;
        }();

        auto projectionMatrix = [&]() -> Matrix4x4 {
            if (mainCamera) {
                return mainCamera->ComputeProjectionMatrix();
            }
            constexpr float distanceToNearPlane = 0.001f;
            constexpr float distanceToFarPlane = 1000.0f;
            return Matrix4x4::CreatePerspectiveFieldOfViewLH(
                MathHelper::ToRadians<float>(45.0f),
                viewport.GetAspectRatio(),
                distanceToNearPlane,
                distanceToFarPlane);
        }();

        this->viewProjection = viewMatrix * projectionMatrix;

        renderer->SetViewMatrix(viewMatrix);
        renderer->SetProjectionMatrix(projectionMatrix);

        needToUpdateViewProjectionMatrix = false;
    }

    renderer->Reset();

    for (auto & entity : entityManager.QueryComponents<GraphicsComponent>()) {
        auto graphicsComponent = entity.GetComponent<GraphicsComponent>();
        POMDOG_ASSERT(graphicsComponent);
        graphicsComponent->Visit(entity, *renderer);
    }

    const bool canSkipPostProcess = postProcessCompositor.CanSkipPostProcess();

    {
        // Reset graphics commands
        commandListPreRender->Reset();

        // Fill background color
        auto backgroundColor = [&]()-> Color {
            if (mainCamera) {
                return mainCamera->GetBackgroundColor();
            }
            return Color::CornflowerBlue;
        }();

        // Set the render pass
        RenderPass renderPass;
        if (canSkipPostProcess) {
            renderPass.RenderTargets.emplace_back(nullptr, backgroundColor.ToVector4());
        }
        else {
            renderPass.RenderTargets.emplace_back(renderTarget, backgroundColor.ToVector4());
        }
        renderPass.Viewport = viewport;
        renderPass.ScissorRect = viewport.GetBounds();
        renderPass.ClearDepth = 1.0f;
        renderPass.ClearStencil = 0;
        commandListPreRender->SetRenderPass(std::move(renderPass));

        // Close graphics command list
        commandListPreRender->Close();
    }

    if (!canSkipPostProcess)
    {
        // Reset graphics commands
        commandListPostRender->Reset();

        // Post processing
        postProcessCompositor.Draw(*commandListPostRender, renderTarget);

        // Close graphics command list
        commandListPostRender->Close();
    }

    auto commandQueue = gameHost->GetGraphicsCommandQueue();
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandListPreRender);
    commandQueue->PushbackCommandList(renderer->Render());
    if (!canSkipPostProcess) {
        commandQueue->PushbackCommandList(commandListPostRender);
    }
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

void Simple2DGameEngine::Composite(
    std::vector<std::shared_ptr<ImageEffectBase>> && imageEffects)
{
    postProcessCompositor.Composite(std::move(imageEffects));
}

void Simple2DGameEngine::SetCamera(const Entity& cameraObject)
{
    POMDOG_ASSERT(cameraObject);
    POMDOG_ASSERT(cameraObject.HasComponent<CameraComponent>());
    POMDOG_ASSERT(cameraObject.HasComponent<Transform>());

    this->mainCamera = cameraObject.GetComponent<CameraComponent>();
    this->mainCameraTransform = cameraObject.GetComponent<Transform>();
}

} // namespace Pomdog
