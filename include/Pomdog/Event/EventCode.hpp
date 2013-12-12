//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCODE_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP
#define POMDOG_EVENTCODE_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct EventCodeTag {};

}// namespace Tags
}// namespace Details

using EventCode = Details::Tagged<
	std::uint32_t, Details::Tags::EventCodeTag
>;

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCODE_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP)
