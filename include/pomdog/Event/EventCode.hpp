//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCODE_H
#define POMDOG_EVENTCODE_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Details {

namespace Tags
{
	struct EventCode {};
}// namespace Tags

}// namespace Details

typedef Details::Tagged<
	std::uint32_t, Details::Tags::EventCode
> EventCode;

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCODE_H)
