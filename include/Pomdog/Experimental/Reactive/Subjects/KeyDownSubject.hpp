// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace Pomdog::Reactive {

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

} // namespace Pomdog::Reactive
