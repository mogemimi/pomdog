// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FORWARDDECLARATIONS_7CF2D9B3_HPP
#define POMDOG_FORWARDDECLARATIONS_7CF2D9B3_HPP

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

#endif // POMDOG_FORWARDDECLARATIONS_7CF2D9B3_HPP
