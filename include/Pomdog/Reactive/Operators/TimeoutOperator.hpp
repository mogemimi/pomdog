// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Application/Timer.hpp"
#include "Pomdog/Reactive/Observable.hpp"
#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog {
namespace Reactive {
namespace Detail {

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

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
