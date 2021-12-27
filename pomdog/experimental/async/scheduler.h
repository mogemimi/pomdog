// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/duration.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency {

class POMDOG_EXPORT Scheduler {
public:
    virtual ~Scheduler() noexcept;

    virtual void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime = Duration::zero()) = 0;
};

} // namespace pomdog::concurrency
