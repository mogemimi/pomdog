//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDMESHCOMMAND_E70DE762_89CF_4D78_873D_281192FE5CA5_HPP
#define POMDOG_SKINNEDMESHCOMMAND_E70DE762_89CF_4D78_873D_281192FE5CA5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/SkinnedEffect.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonTransform.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkinnedMesh.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Rendering {

class SkinnedMeshCommand final: public RenderCommand {
public:
	explicit SkinnedMeshCommand(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
		: skinnedEffect(graphicsDevice)
	{}

	void Execute(std::shared_ptr<GraphicsContext> const& graphicsContext) override;

	void SetMatrixPalette(Skeleton const& skeleton, SkeletonTransform const& skeletonTransform);

	RenderCommandType CommandType() const override;

	float DrawOrder() const override
	{
		return drawOrder;
	}

public:
	SkinnedEffect skinnedEffect;
	std::shared_ptr<SkinnedMesh> mesh;
	float drawOrder;
};

}// namespace Rendering
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHCOMMAND_E70DE762_89CF_4D78_873D_281192FE5CA5_HPP)
