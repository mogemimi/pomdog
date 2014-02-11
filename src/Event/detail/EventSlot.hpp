//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_EVENT_EVENTSLOT_8B2CC092_083F_444A_A39A_00594B32E90B_HPP
#define POMDOG_SRC_EVENT_EVENTSLOT_8B2CC092_083F_444A_A39A_00594B32E90B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>

namespace Pomdog {
namespace Details {
namespace EventInternal {

template <typename Function>
using EventSlot = std::function<Function>;

}// namespace EventInternal
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_EVENT_EVENTSLOT_8B2CC092_083F_444A_A39A_00594B32E90B_HPP)
