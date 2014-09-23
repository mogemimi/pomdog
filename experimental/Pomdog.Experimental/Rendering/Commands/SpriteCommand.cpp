//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteCommand.hpp"

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
void SpriteCommand::Execute(GraphicsContext & graphicsContext)
{
	///@todo not implemented
}
//-----------------------------------------------------------------------
RenderCommandType SpriteCommand::CommandType() const
{
	return RenderCommandType::Sprite;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog
