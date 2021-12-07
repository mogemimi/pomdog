// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/signals/signal.hpp"

namespace Pomdog::Detail {

class SubsystemScheduler final {
public:
    SubsystemScheduler() = default;
    SubsystemScheduler(const SubsystemScheduler&) = delete;
    SubsystemScheduler& operator=(const SubsystemScheduler&) = delete;

    Signal<void()> OnUpdate;
};

} // namespace Pomdog::Detail
