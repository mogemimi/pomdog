// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog.Experimental/InGameEditor/detail/EditorBackground.hpp>
#include <Pomdog.Experimental/Skeletal2D/detail/AnimationTimer.hpp>
#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SpriteBatch;
class SpriteRenderer;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class GrassBlendingGame: public Game {
public:
    explicit GrassBlendingGame(std::shared_ptr<GameHost> const& gameHost);

    ~GrassBlendingGame();

    void Initialize();

    void Update();

    void Draw();

private:
    void DrawSprites();
    void DrawSkinnedMesh();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;

    std::shared_ptr<Texture2D> texture;
    std::unique_ptr<SpriteRenderer> spriteRenderer;

    std::shared_ptr<SamplerState> samplerPoint;
    std::shared_ptr<RenderTarget2D> renderTarget;
    std::unique_ptr<FXAA> fxaa;
    std::unique_ptr<ScreenQuad> screenQuad;

    std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
    std::unique_ptr<SceneEditor::EditorBackground> editorBackground;
    std::shared_ptr<UI::ScenePanel> scenePanel;
    std::shared_ptr<UI::Slider> slider1;
    std::shared_ptr<UI::Slider> slider2;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch1;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch2;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch3;
    std::shared_ptr<UI::ToggleSwitch> toggleSwitch4;

    GameWorld gameWorld;
    GameObject mainCamera;

    AnimationSystem animationSystem;
    std::shared_ptr<Skeleton> maidSkeleton;
    std::shared_ptr<SkeletonPose> maidSkeletonPose;
    std::shared_ptr<AnimationState> maidAnimationState;
    std::shared_ptr<Texture2D> maidTexture;
    std::vector<Matrix3x2> maidGlobalPose;
    std::shared_ptr<AnimationClip> maidAnimationClipIdle;
    Detail::Skeletal2D::AnimationTimer maidAnimationTimer;
    Skin maidSkin;
    std::vector<Detail::Skeletal2D::SpriteAnimationTrack> maidSpriteAnimationTracks;

    SkinnedMesh maidSkinnedMesh;
    std::shared_ptr<SkinnedEffect> maidSkinningEffect;

    ConnectionList connections;
    Viewport clientViewport;

    std::unique_ptr<PolygonBatch> polygonBatch;
};

} // namespace TestApp
