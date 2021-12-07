// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/async/task.hpp"
#include "pomdog/signals/signal.hpp"
#include "pomdog/signals/signal_helpers.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Concurrency {

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

} // namespace Pomdog::Concurrency
