// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

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

template <typename Argument> POMDOG_EXPORT
auto FromSingleShotSignal(
    Signal<void(Argument)> & signal,
    std::shared_ptr<Scheduler> const& scheduler)
    -> Task<std::remove_const_t<std::remove_reference_t<Argument>>>
{
    POMDOG_ASSERT(scheduler);
    using TResult = std::remove_const_t<std::remove_reference_t<Argument>>;
    TaskCompletionSource<TResult> tcs(scheduler);
    Signals::ConnectSingleShot(signal, [tcs](TResult const& arg) {
        tcs.SetResult(arg);
    });
    return Concurrency::CreateTask(tcs);
}

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_HELPERS_B842D84E_HPP
