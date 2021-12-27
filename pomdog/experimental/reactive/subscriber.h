// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/reactive/observer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive {

template <class T>
class Subscriber final : public Observer<T> {
public:
    template <class NextAction>
    explicit Subscriber(NextAction&& onNextIn)
        : onNext(std::forward<NextAction>(onNextIn))
    {
        POMDOG_ASSERT(onNext);
    }

    template <class NextAction, class ErrorAction>
    Subscriber(NextAction&& onNextIn, ErrorAction&& onErrorIn)
        : onNext(std::forward<NextAction>(onNextIn))
        , onError(std::forward<ErrorAction>(onErrorIn))
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(onError);
    }

    template <class NextAction, class ErrorAction, class CompletedAction>
    Subscriber(NextAction&& onNextIn, ErrorAction&& onErrorIn, CompletedAction&& onCompletedIn)
        : onNext(std::forward<NextAction>(onNextIn))
        , onError(std::forward<ErrorAction>(onErrorIn))
        , onCompleted(std::forward<CompletedAction>(onCompletedIn))
    {
        POMDOG_ASSERT(onNext);
    }

    void OnNext(T value) override
    {
        if (onNext) {
            onNext(std::move(value));
        }
    }

    void OnError() override
    {
        if (onError) {
            onError();
        }
    }

    void OnCompleted() override
    {
        if (onCompleted) {
            onCompleted();
        }
    }

private:
    std::function<void(const T&)> onNext;
    std::function<void()> onError;
    std::function<void()> onCompleted;
};

} // namespace pomdog::reactive
