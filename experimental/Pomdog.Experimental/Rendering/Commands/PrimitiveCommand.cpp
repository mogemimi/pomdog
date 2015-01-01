//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "PrimitiveCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Details {
namespace Rendering {
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
