//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
std::type_index ParticleBatchCommand::TypeIndex() const
{
	static const std::type_index index = typeid(ParticleBatchCommand);
	return index;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog
