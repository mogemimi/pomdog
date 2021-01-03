// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Helpers.hpp"
#include "Pomdog/Signals/Signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Concurrency {

template <typename Argument> POMDOG_EXPORT
auto FromSingleShotSignal(Signal<void(Argument)> & signal)
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
