// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/Signal.hpp"

namespace Pomdog {
namespace Detail {

class SubsystemScheduler final {
public:
    SubsystemScheduler() = default;
    SubsystemScheduler(const SubsystemScheduler&) = delete;
    SubsystemScheduler& operator=(const SubsystemScheduler&) = delete;

    Signal<void()> OnUpdate;
};

} // namespace Detail
} // namespace Pomdog
