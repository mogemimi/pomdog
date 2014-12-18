//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP
#define POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SkinnedMeshCommand.hpp"
#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SkinnedMeshRenderable: public Renderable {
public:
	SkinnedMeshRenderable(
		std::shared_ptr<Skeleton> const& skeleton,
		std::shared_ptr<SkeletonTransform> const& skeletonTransform,
		std::shared_ptr<SkinnedMesh> const& mesh,
		std::shared_ptr<Texture2D> const& texture);

	void Visit(GameObject & gameObject, Renderer & renderer) override;

private:
	Details::Rendering::SkinnedMeshCommand command;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_C7C5_4782_90D2_DA79EDC75518_HPP)
