// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Signals/Helpers.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

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
