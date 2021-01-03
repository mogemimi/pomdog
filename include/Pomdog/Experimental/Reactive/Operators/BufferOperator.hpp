// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Reactive::Detail {

template <class T>
class BufferOperator final
    : public Observer<T>
    , public Observable<std::vector<T>> {
public:
    explicit BufferOperator(int countIn)
        : count(countIn)
    {
        POMDOG_ASSERT(count > 0);
        events.reserve(count);
    }

    void Subscribe(const std::shared_ptr<Observer<std::vector<T>>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        events.push_back(std::move(value));
        POMDOG_ASSERT(events.size() <= count);
        if (events.size() == count) {
            std::vector<T> temporaryEvents;
            temporaryEvents.reserve(count);
            std::swap(events, temporaryEvents);
            if (observer) {
                observer->OnNext(std::move(temporaryEvents));
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
        if (!events.empty()) {
            std::vector<T> temporaryEvents;
            temporaryEvents.reserve(count);
            std::swap(events, temporaryEvents);
            if (observer) {
                observer->OnNext(std::move(temporaryEvents));
            }
        }
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<std::vector<T>>> observer;
    std::vector<T> events;
    int count;
};

} // namespace Pomdog::Reactive::Detail
