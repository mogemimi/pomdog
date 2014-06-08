//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MAIDCHANGAME_FB2D5E96_5FD9_483C_82E5_352F02568507_HPP
#define POMDOG_MAIDCHANGAME_FB2D5E96_5FD9_483C_82E5_352F02568507_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "SceneEditor/EditorColorScheme.hpp"
#include "SceneEditor/GradientPlane.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/UIElementHierarchy.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "Skeletal2D/Skeleton.hpp"
#include "Skeletal2D/SkeletonPose.hpp"
#include "Skeletal2D/Skin.hpp"
#include "Skeletal2D/AnimationClip.hpp"
#include "Skeletal2D/SpriteAnimationTrack.hpp"
#include "Skeletal2D/AnimationState.hpp"
#include "Skeletal2D/AnimationSystem.hpp"
#include "Skeletal2D/AnimationTimer.hpp"

namespace TestApp {

using namespace Pomdog;

class PrimitiveAxes;
class PrimitiveGrid;
class SpriteRenderer;
class SpriteBatch;
class FXAA;

class MaidChanGame: public Game {
public:
	explicit MaidChanGame(std::shared_ptr<GameHost> const& gameHost);

	~MaidChanGame();

	void Initialize();
	
	void Update();
	
	void Draw();
	
private:
	void DrawSprites();
	void DrawGUI();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
		
	std::shared_ptr<Texture2D> texture;
	std::unique_ptr<PrimitiveAxes> primitiveAxes;
	std::unique_ptr<PrimitiveGrid> primitiveGrid;
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	
	std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	
	SceneEditor::EditorColorScheme editorColorScheme;
	std::unique_ptr<SceneEditor::GradientPlane> backgroundPlane;
	UI::UIElementHierarchy hierarchy;
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	
	GameWorld gameWorld;
	std::shared_ptr<HierarchyNode> rootNode;
	std::shared_ptr<GameObject> mainCamera;
	
	AnimationSystem animationSystem;
	std::shared_ptr<Skeleton> maidSkeleton;
	std::shared_ptr<SkeletonPose> maidSkeletonPose;
	std::shared_ptr<AnimationState> maidAnimationState;
	std::shared_ptr<Texture2D> maidTexture;
	AnimationTimer maidAnimationTimer;
	Skin maidSkin;
	std::vector<Details::Skeletal2D::SpriteAnimationTrack> maidSpriteAnimationTracks;
};

}// namespace TestApp

#endif // !defined(POMDOG_MAIDCHANGAME_FB2D5E96_5FD9_483C_82E5_352F02568507_HPP)
