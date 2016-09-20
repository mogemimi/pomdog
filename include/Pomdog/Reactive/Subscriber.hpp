// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>

namespace Pomdog {
namespace Reactive {

template <class T>
class Subscriber final : public Observer<T> {
public:
    template <class NextAction>
    explicit Subscriber(NextAction && onNextIn)
        : onNext(std::forward<NextAction>(onNextIn))
    {
        POMDOG_ASSERT(onNext);
    }

    template <class NextAction, class ErrorAction>
    Subscriber(NextAction && onNextIn, ErrorAction && onErrorIn)
        : onNext(std::forward<NextAction>(onNextIn))
        , onError(std::forward<ErrorAction>(onErrorIn))
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(onError);
    }

    template <class NextAction, class ErrorAction, class CompletedAction>
    Subscriber(NextAction && onNextIn, ErrorAction && onErrorIn, CompletedAction && onCompletedIn)
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

} // namespace Reactive
} // namespace Pomdog
