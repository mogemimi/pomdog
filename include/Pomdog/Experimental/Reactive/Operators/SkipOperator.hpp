// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog::Reactive::Detail {

template <class T>
class SkipOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit SkipOperator(int remainingIn)
        : remaining(remainingIn)
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
        if (remaining != 0) {
            --remaining;
            return;
        }
        POMDOG_ASSERT(remaining == 0);
        if (observer) {
            observer->OnNext(std::move(value));
        }
    }

    void OnError() override
    {
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    int remaining;
};

} // namespace Pomdog::Reactive::Detail
