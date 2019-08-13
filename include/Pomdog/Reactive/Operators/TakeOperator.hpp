// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Reactive/Observable.hpp"
#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog::Reactive::Detail {

template <class T>
class TakeOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit TakeOperator(int limit)
        : remaining(limit)
    {
        POMDOG_ASSERT(remaining > 0);
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        POMDOG_ASSERT(remaining >= 0);
        if (remaining == 0) {
            return;
        }
        --remaining;
        POMDOG_ASSERT(remaining >= 0);
        if (observer) {
            observer->OnNext(std::move(value));
        }
        if (remaining == 0) {
            if (observer) {
                observer->OnCompleted();
            }
        }
    }

    void OnError() override
    {
        POMDOG_ASSERT(remaining >= 0);
        if (remaining == 0) {
            return;
        }
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        POMDOG_ASSERT(remaining >= 0);
        if (remaining == 0) {
            return;
        }
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    int remaining;
};

} // namespace Pomdog::Reactive::Detail
