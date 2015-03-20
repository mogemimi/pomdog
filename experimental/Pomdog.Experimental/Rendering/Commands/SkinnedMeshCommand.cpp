//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SkinnedMeshCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Detail {
namespace Rendering {
//-----------------------------------------------------------------------
std::type_index SkinnedMeshCommand::TypeIndex() const
{
	static const std::type_index index = typeid(SkinnedMeshCommand);
	return index;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Detail
}// namespace Pomdog
