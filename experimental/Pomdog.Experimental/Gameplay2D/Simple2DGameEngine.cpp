// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Simple2DGameEngine.hpp"
#include "Pomdog.Experimental/Gameplay2D/ActorComponent.hpp"
#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/SpriteBatchCommandProcessor.hpp"

namespace Pomdog {

Simple2DGameEngine::Simple2DGameEngine(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , gameTimer(gameHostIn->GetClock())
    , postProcessCompositor(
        gameHost->GetGraphicsDevice(),
        gameHost->GetWindow()->GetClientBounds().Width,
        gameHost->GetWindow()->GetClientBounds().Height,
        SurfaceFormat::R8G8B8A8_UNorm)
    , needToUpdateViewProjectionMatrix(true)
{
    auto graphicsDevice = gameHost->GetGraphicsDevice();
    auto assets = gameHost->GetAssetManager();
    auto window = gameHost->GetWindow();

    auto clientBounds = window->GetClientBounds();

    postProcessCompositor.SetViewportSize(
        *graphicsDevice, clientBounds.Width, clientBounds.Height);

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
        viewport = {0, 0, bounds.Width, bounds.Height, 0.0f, 1000.0f};
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

    viewport.Width = width;
    viewport.Height = height;

    renderTarget = std::make_shared<RenderTarget2D>(
        graphicsDevice, width, height, false,
        SurfaceFormat::R8G8B8A8_UNorm,
        DepthFormat::Depth24Stencil8);

    postProcessCompositor.SetViewportSize(*graphicsDevice, width, height);

    if (mainCamera) {
        mainCamera->SetViewportSize(width, height);
    }

    needToUpdateViewProjectionMatrix = true;
}

void Simple2DGameEngine::Update()
{
    for (auto & entity : entityManager.QueryComponents<ActorComponent>()) {
        auto actor = entity.GetComponent<ActorComponent>();
        actor->Act(entity, gameTimer.GetFrameDuration());
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
            return Matrix4x4::CreatePerspectiveFieldOfViewLH(
                MathHelper::ToRadians<float>(45.0f),
                viewport.GetAspectRatio(),
                viewport.MinDepth,
                viewport.MaxDepth);
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

        // Set the back buffer as the target
        RenderPass renderPass;
        renderPass.RenderTargets.emplace_back(nullptr, NullOpt);
        renderPass.Viewport = viewport;
        renderPass.ScissorRect = viewport.GetBounds();
        renderPass.ClearDepth = NullOpt;
        renderPass.ClearStencil = NullOpt;
        commandListPostRender->SetRenderPass(std::move(renderPass));

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
