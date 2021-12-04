// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "catch_amalgamated.hpp"
#include <memory>
#include <thread>

namespace Pomdog {

class Executor final {
private:
    GameClock clock;
    IOService service;
    bool exitRequest = false;

public:
    Executor()
        : service(&clock)
    {
        auto err = service.Initialize();
        REQUIRE(err == nullptr);
    }

    ~Executor()
    {
        auto err = service.Shutdown();
        REQUIRE(err == nullptr);
    }

    IOService* GetService()
    {
        return &service;
    }

    void ExitLoop()
    {
        exitRequest = true;
    }

    void RunLoop()
    {
        while (!exitRequest) {
            clock.Tick();
            service.Step();

            // NOTE: I want to suppress energy impact if possible.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

} // namespace Pomdog
