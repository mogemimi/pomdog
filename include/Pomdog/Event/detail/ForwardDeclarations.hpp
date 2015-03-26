// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP
#define POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP

#include <functional>

namespace Pomdog {
namespace Detail {
namespace SignalsAndSlots {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class SignalBody;

}// namespace SignalsAndSlots
}// namespace Detail

class Event;
class EventConnection;
class EventHandler;
class EventQueue;
class ScopedConnection;

template <typename Function>
class Signal;

}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_DETAIL_FORWARDDECLARATIONS_7CF2D9B3_CC0C_49B1_9864_E5B7A2E4BDFF_HPP)
