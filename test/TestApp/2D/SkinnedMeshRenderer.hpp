//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDMESHRENDERER_5A9AD268_5CE3_48D1_AEAD_31767902AC6B_HPP
#define POMDOG_SKINNEDMESHRENDERER_5A9AD268_5CE3_48D1_AEAD_31767902AC6B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "Renderer.hpp"
#include "../Skeletal2D/Skeleton.hpp"
#include "../Skeletal2D/SkeletonPose.hpp"
#include "../Skeletal2D/Skin.hpp"
#include "../Skeletal2D/AnimationClip.hpp"
#include "../Skeletal2D/SpriteAnimationTrack.hpp"
#include "../Skeletal2D/AnimationState.hpp"
#include "../Skeletal2D/AnimationSystem.hpp"
#include "../Skeletal2D/AnimationTimer.hpp"
#include "../Skeletal2D/SkinnedMesh.hpp"
#include "../Graphics/PolygonBatch.hpp"

namespace Pomdog {

class SkinnedMeshRenderer: public Renderer {
public:
	SkinnedMeshRenderer();

	void Visit(GameObject & gameObject, RenderQueue & renderQueue, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;
	
	void SetZOrder(float zOrder) override;
	float GetZOrder() const override;
	void SetVisible(bool isVisible) override;
	bool IsVisible() const override;
	
	void Load(std::shared_ptr<GraphicsDevice> const& graphicsDevice, std::shared_ptr<AssetManager> const& assets);
	void Update(GameClock const& clock);
	
	void DrawSkeleton(std::unique_ptr<PolygonBatch> const& polygonBatch,
		Matrix4x4 const& modelViewProjection);
	void DrawSkinnedMesh(std::shared_ptr<GraphicsContext> const& graphicsContext,
		Matrix4x4 const& modelViewProjection);
	
public:
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
	
	float zOrder;
	bool isVisible;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHRENDERER_5A9AD268_5CE3_48D1_AEAD_31767902AC6B_HPP)
