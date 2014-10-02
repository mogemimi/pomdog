//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveCommand.hpp"

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
void PrimitiveCommand::Execute(GraphicsContext & graphicsContext)
{
}
//-----------------------------------------------------------------------
RenderCommandType PrimitiveCommand::CommandType() const
{
	return RenderCommandType::Primitive;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog
