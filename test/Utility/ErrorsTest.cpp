// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/Errors.hpp"
#include "catch.hpp"
#include <tuple>

using Pomdog::Errors::IOError;

namespace Errors = Pomdog::Errors;

namespace {

std::tuple<int, std::unique_ptr<IOError>> Load(const std::string& filename)
{
    if (filename.empty()) {
        return std::make_tuple(0, Errors::New(std::errc::invalid_argument, "empty name"));
    }
    if (filename != "foo") {
        return std::make_tuple(0, Errors::New(std::errc::no_such_file_or_directory, "not found"));
    }
    return std::make_tuple(42, nullptr);
}

} // namespace

TEST_CASE("IOError", "[IOError]")
{
    SECTION("when filename is empty")
    {
        auto[result, err] = Load("");
        REQUIRE(err != nullptr);
        REQUIRE(err->Kind == std::errc::invalid_argument);
        REQUIRE(err->Reason == "empty name");
    }
    SECTION("when filename is not found")
    {
        auto[result, err] = Load("bar");
        REQUIRE(err != nullptr);
        REQUIRE(err->Kind == std::errc::no_such_file_or_directory);
        REQUIRE(err->Reason == "not found");
    }
    SECTION("when file exists")
    {
        auto[result, err] = Load("foo");
        REQUIRE(err == nullptr);
        REQUIRE(result == 42);
    }
    SECTION("wrapping")
    {
        auto[result, err] = Load("");
        REQUIRE(err != nullptr);

        auto wrapped = Errors::Wrap(std::move(err), "Load() failed");
#if defined(_MSC_VER) && (_MSC_VER > 1700)
        REQUIRE(wrapped->ToString() == "Load() failed: empty name: invalid argument");
#else
        REQUIRE(wrapped->ToString() == "Load() failed: empty name: Invalid argument");
#endif
    }
}
