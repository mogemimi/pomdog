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
class ScanOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit ScanOperator(std::function<T(const T& accumulation, const T& value)>&& accumulatorIn)
        : accumulator(std::move(accumulatorIn))
        , needToInitialize(false)
    {
        POMDOG_ASSERT(accumulator);
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        POMDOG_ASSERT(accumulator);
        if (needToInitialize) {
            accumulation = value;
            needToInitialize = false;
        }
        else {
            accumulation = accumulator(accumulation, value);
        }

        if (observer) {
            observer->OnNext(accumulation);
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
    std::function<T(const T& accumulation, const T& value)> accumulator;
    T accumulation;
    bool needToInitialize;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
