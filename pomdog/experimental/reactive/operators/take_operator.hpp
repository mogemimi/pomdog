// Copyright mogemimi. Distributed under the MIT license.

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
class TakeOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit TakeOperator(int limit)
        : remaining(limit)
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
        if (remaining == 0) {
            return;
        }
        --remaining;
        POMDOG_ASSERT(remaining >= 0);
        if (observer) {
            observer->OnNext(std::move(value));
        }
        if (remaining == 0) {
            if (observer) {
                observer->OnCompleted();
            }
        }
    }

    void OnError() override
    {
        POMDOG_ASSERT(remaining >= 0);
        if (remaining == 0) {
            return;
        }
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        POMDOG_ASSERT(remaining >= 0);
        if (remaining == 0) {
            return;
        }
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    int remaining;
};

} // namespace Pomdog::Reactive::Detail
