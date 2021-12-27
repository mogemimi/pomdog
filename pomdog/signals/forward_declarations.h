// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::signals {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class SignalBody;

template <typename Function>
class DelegateBody;

} // namespace pomdog::detail::signals

namespace pomdog {

template <typename Event>
class EventQueue;

class Connection;
class ScopedConnection;

class ConnectionList;

template <typename Function>
class Signal;

template <typename Function>
class Delegate;

} // namespace pomdog
