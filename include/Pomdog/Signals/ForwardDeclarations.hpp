// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
namespace Detail::Signals {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class SignalBody;

template <typename Function>
class DelegateBody;

} // namespace Detail::Signals

template <typename Event>
class EventQueue;

class Connection;
class ScopedConnection;

class ConnectionList;

template <typename Function>
class Signal;

template <typename Function>
class Delegate;

} // namespace Pomdog
