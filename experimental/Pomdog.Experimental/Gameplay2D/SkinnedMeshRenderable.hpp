//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP
#define POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Renderable.hpp"
#include "../Rendering/SkinnedMeshCommand.hpp"
#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SkinnedMeshRenderable: public Renderable {
public:
	SkinnedMeshRenderable(
		GraphicsDevice & graphicsDevice,
		std::shared_ptr<Skeleton> const& skeleton,
		std::shared_ptr<SkeletonTransform> const& skeletonTransform,
		std::shared_ptr<SkinnedMesh> const& mesh,
		std::shared_ptr<Texture2D> const& texture);

	void Visit(GameObject & gameObject, Renderer & renderer, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;

	void DrawSkeleton(std::unique_ptr<PolygonBatch> const& polygonBatch,
		Matrix4x4 const& modelViewProjection);

public:
	Details::Rendering::SkinnedMeshCommand command;
	
	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<SkeletonTransform> skeletonTransform;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP)
