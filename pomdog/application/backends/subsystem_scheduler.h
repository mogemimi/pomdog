// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/signals/signal.h"

namespace pomdog::detail {

class SubsystemScheduler final {
public:
    SubsystemScheduler() = default;
    SubsystemScheduler(const SubsystemScheduler&) = delete;
    SubsystemScheduler& operator=(const SubsystemScheduler&) = delete;

    Signal<void()> onUpdate;
};

} // namespace pomdog::detail
