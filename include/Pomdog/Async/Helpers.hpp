// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_HELPERS_B842D84E_HPP
#define POMDOG_HELPERS_B842D84E_HPP

#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Signals/Helpers.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Concurrency {

template <typename TResult> POMDOG_EXPORT
Task<std::remove_const_t<std::remove_reference_t<TResult>>>
FromSingleShotSignal(
    Signal<void(TResult)> & signal,
    std::shared_ptr<Scheduler> const& scheduler)
{
    POMDOG_ASSERT(scheduler);
    auto tcs = std::make_shared<TaskCompletionSource<TResult>>(scheduler);
    Signals::ConnectSingleShot(signal, [tcs](TResult const& arg) {
        tcs->SetResult(arg);
    });
    return tcs->GetTask();
}

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_HELPERS_B842D84E_HPP
