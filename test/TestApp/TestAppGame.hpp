//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOATESTGAME_HPP
#define POMDOG_COCOATESTGAME_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "CameraView2D.hpp"
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

class TestAppGame: public Game {
public:
	explicit TestAppGame(std::shared_ptr<GameHost> const& gameHost);

	~TestAppGame();

	void Initialize();
	
	void Update();
	
	void Draw();
	
private:
	void DrawSprites();

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
	
	CameraView2D cameraView;
	
	GameWorld gameWorld;
	GameObjectID mainCameraID;
	GameObjectID rootObjectID;
	//std::shared_ptr<Scene> hierarchy;
	
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

#endif // !defined(POMDOG_COCOATESTGAME_HPP)
