// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/game_clock.hpp"
#include "pomdog/chrono/detail/game_clock_impl.hpp"
#include "pomdog/chrono/detail/make_time_source.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/network/io_service.hpp"
#include <catch_amalgamated.hpp>
#include <memory>
#include <thread>

namespace pomdog {

class Executor final {
private:
    std::shared_ptr<detail::TimeSource> timeSource_;
    std::shared_ptr<detail::GameClockImpl> clock_;
    std::unique_ptr<IOService> service_;
    bool exitRequest_ = false;

public:
    Executor()
    {
        timeSource_ = detail::makeTimeSource();
        clock_ = std::make_shared<detail::GameClockImpl>();
        REQUIRE(clock_->Initialize(60, timeSource_) == nullptr);

        service_ = std::make_unique<IOService>();

        auto err = service_->Initialize(clock_);
        REQUIRE(err == nullptr);
    }

    ~Executor()
    {
        auto err = service_->Shutdown();
        REQUIRE(err == nullptr);
    }

    IOService* GetService()
    {
        return service_.get();
    }

    void ExitLoop()
    {
        exitRequest_ = true;
    }

    void RunLoop()
    {
        while (!exitRequest_) {
            clock_->Tick();
            service_->Step();

            // NOTE: I want to suppress energy impact if possible.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

} // namespace pomdog
