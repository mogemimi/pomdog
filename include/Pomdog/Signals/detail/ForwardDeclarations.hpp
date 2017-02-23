// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include <functional>

namespace Pomdog {
namespace Detail {
namespace Signals {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class SignalBody;

} // namespace Signals
} // namespace Detail

class Event;
class EventQueue;

class Connection;
class ScopedConnection;

class ConnectionList;

template <typename Function>
class Signal;

} // namespace Pomdog
