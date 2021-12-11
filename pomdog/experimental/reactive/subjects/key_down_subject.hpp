// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/chrono/game_clock.hpp"
#include "pomdog/experimental/reactive/observable.hpp"
#include "pomdog/experimental/reactive/observer.hpp"
#include "pomdog/input/keyboard.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive {

class KeyDownSubject final : public Observable<Keys> {
public:
    explicit KeyDownSubject(const std::shared_ptr<GameHost>& gameHost)
    {
        POMDOG_ASSERT(gameHost);
        auto keyboard = gameHost->GetKeyboard();

        POMDOG_ASSERT(keyboard);
        connection = keyboard->KeyDown.Connect([this](Keys key) {
            if (observer) {
                observer->OnNext(key);
            }
        });
    }

    void Subscribe(const std::shared_ptr<Observer<Keys>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

private:
    std::shared_ptr<Observer<Keys>> observer;
    ScopedConnection connection;
};

} // namespace pomdog::reactive
