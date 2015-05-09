// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SUBSYSTEMSCHEDULER_15398B34_HPP
#define POMDOG_SUBSYSTEMSCHEDULER_15398B34_HPP

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Signals/Signal.hpp"

namespace Pomdog {
namespace Detail {

class SubsystemScheduler: Noncopyable {
public:
    Signal<void()> OnUpdate;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SUBSYSTEMSCHEDULER_15398B34_HPP
