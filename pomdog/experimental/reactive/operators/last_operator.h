// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/reactive/observable.h"
#include "pomdog/experimental/reactive/observer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive::detail {

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

} // namespace pomdog::reactive::detail
