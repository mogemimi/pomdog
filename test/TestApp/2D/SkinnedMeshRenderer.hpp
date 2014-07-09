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
#include "../Skeletal2D/SkeletonTransform.hpp"
#include "../Skeletal2D/SkinnedMesh.hpp"
#include "../Graphics/PolygonBatch.hpp"

namespace Pomdog {

class SkinnedMeshRenderer: public Renderer {
public:
	SkinnedMeshRenderer(
		std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::shared_ptr<AssetManager> const& assets,
		std::shared_ptr<Skeleton> const& skeleton,
		std::shared_ptr<SkeletonTransform> const& skeletonTransform,
		std::shared_ptr<SkinnedMesh> const& mesh,
		std::shared_ptr<Texture2D> const& texture);

	void Visit(GameObject & gameObject, RenderQueue & renderQueue, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;
	
	void SetZOrder(float zOrder) override;
	float GetZOrder() const override;
	void SetVisible(bool isVisible) override;
	bool IsVisible() const override;
	
	void DrawSkeleton(std::unique_ptr<PolygonBatch> const& polygonBatch,
		Matrix4x4 const& modelViewProjection);
	void DrawSkinnedMesh(std::shared_ptr<GraphicsContext> const& graphicsContext,
		Matrix4x4 const& modelViewProjection);
	
public:
	std::shared_ptr<SkinnedMesh> mesh;
	std::shared_ptr<Texture2D> texture;
	std::shared_ptr<EffectPass> skinningEffect;
	std::shared_ptr<InputLayout> inputLayout;
	
	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<SkeletonTransform> skeletonTransform;
	
	float zOrder;
	bool isVisible;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHRENDERER_5A9AD268_5CE3_48D1_AEAD_31767902AC6B_HPP)
