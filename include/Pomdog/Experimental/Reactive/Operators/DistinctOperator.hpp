// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog::Reactive::Detail {

template <class T>
class DistinctOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    DistinctOperator()
        : hasValue(false)
    {
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        if (hasValue && (value == lastValue)) {
            return;
        }
        lastValue = value;
        hasValue = true;
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
    T lastValue;
    bool hasValue;
    std::shared_ptr<Observer<T>> observer;
};

} // namespace Pomdog::Reactive::Detail
