// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Signals/Signal.hpp"

namespace Pomdog {
namespace Detail {

class SubsystemScheduler : Noncopyable {
public:
    Signal<void()> OnUpdate;
};

} // namespace Detail
} // namespace Pomdog
