//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP
#define POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>

namespace Pomdog {

class GraphicsContext;

enum class RenderCommandType {
	Custom,
	ParticleBatch,
	Primitive,
	Sprite,
};

class RenderCommand {
public:
	virtual ~RenderCommand() = default;
	
	virtual void Execute(GraphicsContext & graphicsContext) = 0;
	
	virtual RenderCommandType CommandType() const = 0;
	
	virtual float DrawOrder() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP)
