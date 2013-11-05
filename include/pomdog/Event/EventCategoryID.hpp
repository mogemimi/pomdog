//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCATEGORYID_H
#define POMDOG_EVENTCATEGORYID_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Details {

namespace Tags
{
	struct EventCategoryID {};
}// namespace Tags

}// namespace Details

typedef Details::Tagged<
	std::uint32_t, Details::Tags::EventCategoryID
> EventCategoryID;

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCATEGORYID_H)
