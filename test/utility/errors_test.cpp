// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/errors.h"
#include <catch_amalgamated.hpp>
#include <tuple>

using pomdog::errors::IOError;

namespace errors = pomdog::errors;

namespace {

std::tuple<int, std::unique_ptr<IOError>> Load(const std::string& filename)
{
    if (filename.empty()) {
        return std::make_tuple(0, errors::makeIOError(std::errc::invalid_argument, "empty name"));
    }
    if (filename != "foo") {
        return std::make_tuple(0, errors::makeIOError(std::errc::no_such_file_or_directory, "not found"));
    }
    return std::make_tuple(42, nullptr);
}

} // namespace

TEST_CASE("IOError", "[IOError]")
{
    SECTION("when filename is empty")
    {
        auto [result, err] = Load("");
        REQUIRE(err != nullptr);
        REQUIRE(err->kind == std::errc::invalid_argument);
        REQUIRE(err->reason == "empty name");
    }
    SECTION("when filename is not found")
    {
        auto [result, err] = Load("bar");
        REQUIRE(err != nullptr);
        REQUIRE(err->kind == std::errc::no_such_file_or_directory);
        REQUIRE(err->reason == "not found");
    }
    SECTION("when file exists")
    {
        auto [result, err] = Load("foo");
        REQUIRE(err == nullptr);
        REQUIRE(result == 42);
    }
    SECTION("wrapping")
    {
        auto [result, err] = Load("");
        REQUIRE(err != nullptr);

        auto wrapped = errors::wrap(std::move(err), "Load() failed");
#if defined(_MSC_VER) && (_MSC_VER > 1700)
        REQUIRE(wrapped->toString() == "Load() failed: empty name: invalid argument");
#else
        REQUIRE(wrapped->toString() == "Load() failed: empty name: Invalid argument");
#endif
    }
}
