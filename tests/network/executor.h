// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog {

class Executor final {
private:
    std::shared_ptr<detail::TimeSource> timeSource_;
    std::shared_ptr<detail::GameClockImpl> clock_;
    std::unique_ptr<IOService> service_;
    bool exitRequest_ = false;

public:
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;
    Executor(Executor&&) = delete;
    Executor& operator=(Executor&&) = delete;

    Executor()
    {
        timeSource_ = detail::makeTimeSource();
        clock_ = std::make_shared<detail::GameClockImpl>();
        REQUIRE(clock_->initialize(60, timeSource_) == nullptr);

        service_ = std::make_unique<IOService>();

        auto err = service_->initialize(clock_);
        REQUIRE(err == nullptr);
    }

    ~Executor()
    {
        auto err = service_->shutdown();
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
            clock_->tick();
            service_->step();

            // NOTE: I want to suppress energy impact if possible.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

} // namespace pomdog
