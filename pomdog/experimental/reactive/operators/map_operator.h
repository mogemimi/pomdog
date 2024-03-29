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

} // namespace pomdog::reactive::detail
