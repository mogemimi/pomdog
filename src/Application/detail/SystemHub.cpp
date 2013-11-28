//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/detail/SystemHub.hpp>
#include <utility>

namespace Pomdog {
namespace Details {

static std::weak_ptr<SystemHub> globalSystemHub;

//-----------------------------------------------------------------------
void ResetGlobalSystemHub(std::weak_ptr<SystemHub> systemHub)
{
	globalSystemHub = std::move(systemHub);
}
//-----------------------------------------------------------------------
void ResetGlobalSystemHub()
{
	globalSystemHub.reset();
}
//-----------------------------------------------------------------------
std::weak_ptr<SystemHub> GlobalSystemHub()
{
	return globalSystemHub;
}

}// namespace Details
}// namespace Pomdog
