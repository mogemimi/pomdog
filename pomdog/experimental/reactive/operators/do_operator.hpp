// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/reactive/observable.hpp"
#include "pomdog/experimental/reactive/observer.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive::detail {

template <class T>
class DoOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    template <class NextAction>
    explicit DoOperator(NextAction&& onNextIn)
        : onNext(std::forward<NextAction>(onNextIn))
    {
        POMDOG_ASSERT(onNext);
    }

    template <class NextAction, class ErrorAction>
    DoOperator(NextAction&& onNextIn, ErrorAction&& onErrorIn)
        : onNext(std::forward<NextAction>(onNextIn))
        , onError(std::forward<ErrorAction>(onErrorIn))
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(onError);
    }

    template <class NextAction, class ErrorAction, class CompletedAction>
    DoOperator(NextAction&& onNextIn, ErrorAction&& onErrorIn, CompletedAction&& onCompletedIn)
        : onNext(std::forward<NextAction>(onNextIn))
        , onError(std::forward<ErrorAction>(onErrorIn))
        , onCompleted(std::forward<CompletedAction>(onCompletedIn))
    {
        POMDOG_ASSERT(onNext);
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        if (onNext) {
            onNext(value);
        }
        if (observer) {
            observer->OnNext(value);
        }
    }

    void OnError() override
    {
        if (onError) {
            onError();
        }
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        if (onCompleted) {
            onCompleted();
        }
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    std::function<void(const T&)> onNext;
    std::function<void()> onError;
    std::function<void()> onCompleted;
};

} // namespace pomdog::reactive::detail
