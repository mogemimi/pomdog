// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/application/game_clock.hpp"
#include "pomdog/application/game_host.hpp"
#include "pomdog/application/timer.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/Reactive/Observable.hpp"
#include "pomdog/experimental/Reactive/Observer.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Reactive::Detail {

template <class T>
class TimeoutOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    TimeoutOperator(const Duration& dueTimeIn, const std::shared_ptr<GameHost>& gameHost)
        : timer(gameHost->GetClock())
        , dueTime(dueTimeIn)
        , isStopped(false)
    {
        POMDOG_ASSERT(dueTime >= Duration::zero());

        timer.Stop();
        timer.SetSingleShot(true);
        timer.SetInterval(dueTime);

        connection = timer.Elapsed.Connect([this] {
            POMDOG_ASSERT(!isStopped);
            this->OnError();
        });
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        if (isStopped) {
            return;
        }

        timer.Stop();
        timer.Reset();
        timer.Start();

        if (observer) {
            observer->OnNext(std::move(value));
        }
    }

    void OnError() override
    {
        if (isStopped) {
            return;
        }

        isStopped = true;
        timer.Stop();

        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        if (isStopped) {
            return;
        }

        isStopped = true;
        timer.Stop();

        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    Timer timer;
    ScopedConnection connection;
    Duration dueTime;
    bool isStopped;
};

} // namespace Pomdog::Reactive::Detail
