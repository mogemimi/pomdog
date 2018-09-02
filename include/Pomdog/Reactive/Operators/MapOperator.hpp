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

template <class T, class TResult>
class MapOperator final
    : public Observer<T>
    , public Observable<TResult> {
public:
    explicit MapOperator(std::function<TResult(T&& value)>&& mapIn)
        : map(std::move(mapIn))
    {
        POMDOG_ASSERT(map);
    }

    void Subscribe(const std::shared_ptr<Observer<TResult>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        POMDOG_ASSERT(map);
        if (observer) {
            observer->OnNext(map(std::move(value)));
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
    std::shared_ptr<Observer<TResult>> observer;
    std::function<TResult(T&& value)> map;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
