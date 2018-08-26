// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay2D/CameraComponent.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog.Experimental/Gameplay/EntityManager.hpp"
#include "Pomdog.Experimental/ImageEffects/PostProcessCompositor.hpp"
#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class Simple2DGameEngine final {
public:
    explicit Simple2DGameEngine(const std::shared_ptr<GameHost>& gameHost);

    ~Simple2DGameEngine();

    void Update();

    void Draw();

    void Composite(std::vector<std::shared_ptr<ImageEffectBase>> && imageEffects);

    void SetCamera(const Entity& cameraObject);

private:
    void OnViewportSizeChanged(int width, int height);

private:
    std::shared_ptr<GameHost> gameHost;
    ConnectionList connections;

    PostProcessCompositor postProcessCompositor;
    std::shared_ptr<GraphicsCommandList> commandListPreRender;
    std::shared_ptr<GraphicsCommandList> commandListPostRender;
    std::shared_ptr<RenderTarget2D> renderTarget;
    Matrix4x4 viewProjection;
    Viewport viewport;

    std::shared_ptr<CameraComponent> mainCamera;
    std::shared_ptr<Transform> mainCameraTransform;

    bool needToUpdateViewProjectionMatrix;

public:
    EntityManager entityManager;
    std::unique_ptr<Renderer> renderer;
};

} // namespace Pomdog
