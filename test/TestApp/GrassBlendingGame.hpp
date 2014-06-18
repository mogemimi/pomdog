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
#include "Skeletal2D/Skeleton.hpp"
#include "Skeletal2D/SkeletonPose.hpp"
#include "Skeletal2D/Skin.hpp"
#include "Skeletal2D/AnimationClip.hpp"
#include "Skeletal2D/SpriteAnimationTrack.hpp"
#include "Skeletal2D/AnimationState.hpp"
#include "Skeletal2D/AnimationSystem.hpp"
#include "Skeletal2D/AnimationTimer.hpp"


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

#endif // !defined(POMDOG_GRASSBLENDINGGAME_544D7F2A_1541_499F_84A6_3C16FBA45699_HPP)
