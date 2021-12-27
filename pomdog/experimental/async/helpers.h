// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/async/task.h"
#include "pomdog/signals/signal.h"
#include "pomdog/signals/signal_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency {

template <typename Argument>
[[nodiscard]] POMDOG_EXPORT auto
FromSingleShotSignal(Signal<void(Argument)>& signal)
    -> Task<std::remove_const_t<std::remove_reference_t<Argument>>>
{
    using TResult = std::remove_const_t<std::remove_reference_t<Argument>>;
    TaskCompletionSource<TResult> tcs;
    Signals::ConnectSingleShot(signal, [tcs](const TResult& arg) {
        tcs.SetResult(arg);
    });
    Task<TResult> task(std::move(tcs));
    return task;
}

} // namespace pomdog::concurrency
