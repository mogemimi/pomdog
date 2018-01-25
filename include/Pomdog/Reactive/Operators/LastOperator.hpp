// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Reactive/Observable.hpp"
#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog {
namespace Reactive {
namespace Detail {

template <class T>
class LastOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit LastOperator()
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
        lastValue = std::move(value);
        hasValue = true;
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
            if (hasValue) {
                observer->OnNext(std::move(lastValue));
            }
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    T lastValue;
    bool hasValue;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
