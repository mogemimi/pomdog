// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.hpp"
#include "pomdog/application/timer.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/chrono/duration.hpp"
#include "pomdog/chrono/game_clock.hpp"
#include "pomdog/experimental/Reactive/Observable.hpp"
#include "pomdog/experimental/Reactive/Observer.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive::detail {

template <class T>
class DebounceOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    DebounceOperator(const Duration& dueTimeIn, const std::shared_ptr<GameHost>& gameHost)
        : timer(gameHost->GetClock())
        , dueTime(dueTimeIn)
        , hasValue(false)
    {
        POMDOG_ASSERT(dueTime >= Duration::zero());
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        if (hasValue) {
            connection.Disconnect();
        }
        lastValue = std::move(value);
        hasValue = true;

        timer.Stop();
        timer.Reset();
        timer.SetSingleShot(true);
        timer.SetInterval(dueTime);
        timer.Start();

        connection = timer.Elapsed.Connect([this] {
            POMDOG_ASSERT(hasValue);
            if (observer) {
                observer->OnNext(std::move(lastValue));
            }
            connection.Disconnect();
        });
    }

    void OnError() override
    {
        // TODO: Not implemented
        POMDOG_ASSERT(false);
    }

    void OnCompleted() override
    {
        // TODO: Not implemented
        POMDOG_ASSERT(false);
    }

private:
    std::shared_ptr<Observer<T>> observer;
    Timer timer;
    ScopedConnection connection;
    Duration dueTime;
    T lastValue;
    bool hasValue;
};

} // namespace pomdog::reactive::detail
