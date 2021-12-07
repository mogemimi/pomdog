// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Concurrency {

class POMDOG_EXPORT Scheduler {
public:
    virtual ~Scheduler() noexcept;

    virtual void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime = Duration::zero()) = 0;
};

} // namespace Pomdog::Concurrency
