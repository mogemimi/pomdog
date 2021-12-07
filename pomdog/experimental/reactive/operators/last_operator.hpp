// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/Reactive/Observable.hpp"
#include "pomdog/experimental/Reactive/Observer.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Reactive::Detail {

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

} // namespace Pomdog::Reactive::Detail
