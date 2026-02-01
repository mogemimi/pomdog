// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Error;
using pomdog::errors::make;
using pomdog::errors::makeIOError;
using pomdog::errors::wrap;

TEST_CASE("Errors")
{
    SUBCASE("make")
    {
        auto err = make("test error");
        REQUIRE(err != nullptr);
        REQUIRE(err->toString() == "test error");
    }
    SUBCASE("makeIOError")
    {
        auto err = makeIOError(std::errc::io_error, "io error occurred");
        REQUIRE(err != nullptr);
        REQUIRE(err->toString().find("io error occurred") != std::string::npos);
    }
    SUBCASE("wrap")
    {
        auto err = make("inner error");
        auto wrapped = wrap(std::move(err), "outer error");
        REQUIRE(wrapped != nullptr);

        auto message = wrapped->toString();
        REQUIRE(message.find("outer error") != std::string::npos);
        REQUIRE(message.find("inner error") != std::string::npos);
    }
    SUBCASE("wrap with nullptr")
    {
        std::unique_ptr<Error> err = nullptr;
        auto wrapped = wrap(std::move(err), "outer error");
        // NOTE: wrap creates a WrappedError even with nullptr inner error
        REQUIRE(wrapped != nullptr);
        REQUIRE(wrapped->toString().find("outer error") != std::string::npos);
    }
    SUBCASE("nullptr is no error")
    {
        std::unique_ptr<Error> err = nullptr;
        REQUIRE(err == nullptr);
        REQUIRE_FALSE(err);
    }
}
