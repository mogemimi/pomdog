//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_SYSTEMHUB_HPP
#define POMDOG_DETAIL_SYSTEMHUB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>

namespace Pomdog {

class GraphicsContext;

namespace Details {

class SystemHub: public std::enable_shared_from_this<SystemHub>
{
public:
	std::weak_ptr<GraphicsContext> GraphicsContext;
};

void ResetGlobalSystemHub(std::weak_ptr<SystemHub> systemHub);
void ResetGlobalSystemHub();

std::weak_ptr<SystemHub> GlobalSystemHub();

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_SYSTEMHUB_HPP)
