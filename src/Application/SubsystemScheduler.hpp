// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SUBSYSTEMSCHEDULER_15398B34_AE1E_4AAA_BE2A_EC942196DC84_HPP
#define POMDOG_SUBSYSTEMSCHEDULER_15398B34_AE1E_4AAA_BE2A_EC942196DC84_HPP

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Event/Signal.hpp"

namespace Pomdog {
namespace Detail {

class SubsystemScheduler: Noncopyable {
public:
    Signal<void()> OnUpdate;
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SUBSYSTEMSCHEDULER_15398B34_AE1E_4AAA_BE2A_EC942196DC84_HPP)
