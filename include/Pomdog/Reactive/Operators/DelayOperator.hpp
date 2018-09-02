// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Reactive/Observable.hpp"
#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Reactive {
namespace Detail {

template <class T>
class DelayOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    DelayOperator(const Duration& intervalIn, const std::shared_ptr<GameHost>& gameHost)
        : interval(intervalIn)
    {
        POMDOG_ASSERT(interval >= Duration::zero());

        POMDOG_ASSERT(gameHost);
        auto clock = gameHost->GetClock();

        POMDOG_ASSERT(clock);
        connection = clock->OnTick.Connect([this](const Duration& frameDuration) {
            if (events.empty()) {
                return;
            }

            POMDOG_ASSERT(interval >= Duration::zero());
            auto iter = events.begin();
            while (iter != events.end()) {
                auto& event = *iter;

                // TODO: kahan velvel algorithm
                event.totalTime += frameDuration;

                if (event.totalTime >= interval) {
                    if (observer) {
                        observer->OnNext(std::move(std::move(event.value)));
                    }
                    iter = events.erase(iter);
                }
                else {
                    ++iter;
                }
            }
        });
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        DelayedEvent event;
        event.value = std::move(value);
        event.totalTime = Duration::zero();
        events.push_back(std::move(event));
    }

    void OnError() override
    {
        // TODO: Not implemented
        POMDOG_ASSERT(false);
    }

    void OnCompleted() override
    {
        // TODO: Not implemented
        POMDOG_ASSERT(false);
    }

private:
    struct DelayedEvent {
        T value;
        Duration totalTime;
    };
    std::shared_ptr<Observer<T>> observer;
    std::vector<DelayedEvent> events;
    ScopedConnection connection;
    Duration interval;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
