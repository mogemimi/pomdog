//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GRASSBLENDINGGAME_544D7F2A_1541_499F_84A6_3C16FBA45699_HPP
#define POMDOG_GRASSBLENDINGGAME_544D7F2A_1541_499F_84A6_3C16FBA45699_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "SceneEditor/InGameEditor.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/UIElementHierarchy.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "UI/ToggleSwitch.hpp"
#include "Skeletal2D/Skeleton.hpp"
#include "Skeletal2D/SkeletonPose.hpp"
#include "Skeletal2D/Skin.hpp"
#include "Skeletal2D/AnimationClip.hpp"
#include "Skeletal2D/SpriteAnimationTrack.hpp"
#include "Skeletal2D/AnimationState.hpp"
#include "Skeletal2D/AnimationSystem.hpp"
#include "Skeletal2D/AnimationTimer.hpp"
#include "Skeletal2D/SkinnedMesh.hpp"
#include "Graphics/PolygonBatch.hpp"
#include "Graphics/SolidShapeBatch.hpp"


namespace Pomdog {

class SpriteBatch;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class SpriteRenderer;
class FXAA;

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
	std::shared_ptr<GraphicsContext> graphicsContext;
		
	std::shared_ptr<Texture2D> texture;
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	
	std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;

	std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch1;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch2;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch3;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch4;

	GameWorld gameWorld;
	std::shared_ptr<HierarchyNode> rootNode;
	std::shared_ptr<GameObject> mainCamera;

	AnimationSystem animationSystem;
	std::shared_ptr<Skeleton> maidSkeleton;
	std::shared_ptr<SkeletonPose> maidSkeletonPose;
	std::shared_ptr<AnimationState> maidAnimationState;
	std::shared_ptr<Texture2D> maidTexture;
	std::vector<Matrix3x2> maidGlobalPose;
	std::shared_ptr<AnimationClip> maidAnimationClipIdle;
	AnimationTimer maidAnimationTimer;
	Skin maidSkin;
	std::vector<Details::Skeletal2D::SpriteAnimationTrack> maidSpriteAnimationTracks;
	
	SkinnedMesh maidSkinnedMesh;
	std::shared_ptr<EffectPass> maidSkinningEffect;
	std::shared_ptr<InputLayout> maidInputLayout;
	
	ScopedConnection clientSizeChangedConnection;
	
	std::unique_ptr<PolygonBatch> polygonBatch;
};

}// namespace TestApp

#endif // !defined(POMDOG_GRASSBLENDINGGAME_544D7F2A_1541_499F_84A6_3C16FBA45699_HPP)
