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
class FilterOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit FilterOperator(std::function<bool(const T& value)>&& filterIn)
        : filter(std::move(filterIn))
    {
        POMDOG_ASSERT(filter);
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        POMDOG_ASSERT(filter);
        if (filter(value)) {
            if (observer) {
                observer->OnNext(std::move(value));
            }
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
    std::function<bool(const T& value)> filter;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
