//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP
#define POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>

namespace Pomdog {
namespace Details {
namespace SignalsAndSlots {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class Signal;

}// namespace SignalsAndSlots
}// namespace Details

class Event;
class EventConnection;
class EventHandler;
class EventQueue;
class ScopedConnection;

}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP)
