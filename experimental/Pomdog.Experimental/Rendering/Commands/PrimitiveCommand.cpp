//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
void PrimitiveCommand::Execute(GraphicsContext & graphicsContext)
{
}
//-----------------------------------------------------------------------
std::type_index PrimitiveCommand::TypeIndex() const
{
	static const std::type_index index = typeid(PrimitiveCommand);
	return index;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog
