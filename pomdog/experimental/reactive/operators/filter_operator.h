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

} // namespace pomdog::reactive::detail
