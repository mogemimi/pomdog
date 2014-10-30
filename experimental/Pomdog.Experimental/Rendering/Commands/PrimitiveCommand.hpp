//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PRIMITIVECOMMAND_4CCAFA9C_9995_45A4_8646_411676A87B19_HPP
#define POMDOG_PRIMITIVECOMMAND_4CCAFA9C_9995_45A4_8646_411676A87B19_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Rendering {

class PrimitiveCommand final: public RenderCommand {
public:
	float DrawOrder() const override
	{
		return drawOrder;
	}

	void Execute(GraphicsContext & graphicsContext) override;
	
	std::type_index TypeIndex() const override;

public:
	Matrix3x2 transform;
	//Vector2 originPivot;
	Rectangle rectangle;
	Color leftTopColor;
	Color rightTopColor;
	Color leftBottomColor;
	Color rightBottomColor;
	float drawOrder;
};

}// namespace Rendering
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PRIMITIVECOMMAND_4CCAFA9C_9995_45A4_8646_411676A87B19_HPP)
