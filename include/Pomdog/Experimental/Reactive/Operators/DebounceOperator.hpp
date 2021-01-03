// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Application/Timer.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Reactive::Detail {

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

} // namespace Pomdog::Reactive::Detail
