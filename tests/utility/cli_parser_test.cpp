// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/cli_parser.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::CLIParser;
using pomdog::i32;
using pomdog::u32;
using pomdog::u64;

TEST_CASE("CLIParser.Bool")
{
    CLIParser parser;

    bool verbose = false;
    parser.add(&verbose, "verbose", "enable verbose output");

    SUBCASE("default value is false")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(verbose == false);
    }
    SUBCASE("flag sets to true")
    {
        const char* const argv[] = {"prog", "-verbose"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(verbose == true);
    }
}

TEST_CASE("CLIParser.I32")
{
    CLIParser parser;
    i32 count = 0;
    parser.add(&count, "count", "number of items");

    SUBCASE("default value is 0")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(count == 0);
    }
    SUBCASE("parse positive value")
    {
        const char* const argv[] = {"prog", "-count", "42"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(count == 42);
    }
    SUBCASE("parse negative value")
    {
        const char* const argv[] = {"prog", "-count", "-7"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(count == -7);
    }
    SUBCASE("invalid value returns error")
    {
        const char* const argv[] = {"prog", "-count", "abc"};
        auto err = parser.parse(argv);
        REQUIRE(err != nullptr);
    }
    SUBCASE("missing value returns error")
    {
        const char* const argv[] = {"prog", "-count"};
        auto err = parser.parse(argv);
        REQUIRE(err != nullptr);
    }
}

TEST_CASE("CLIParser.U32")
{
    CLIParser parser;
    u32 port = 0;
    parser.add(&port, "port", "port number");

    SUBCASE("default value is 0")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(port == 0);
    }
    SUBCASE("parse value")
    {
        const char* const argv[] = {"prog", "-port", "8080"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(port == 8080);
    }
}

TEST_CASE("CLIParser.U64")
{
    CLIParser parser;
    u64 seed = 0;
    parser.add(&seed, "seed", "hash seed");

    SUBCASE("default value is 0")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(seed == 0);
    }
    SUBCASE("parse large value")
    {
        const char* const argv[] = {"prog", "-seed", "20160723"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(seed == 20160723);
    }
}

TEST_CASE("CLIParser.String")
{
    CLIParser parser;
    std::string name;
    parser.add(&name, "name", "user name");

    SUBCASE("default value is empty")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(name.empty());
    }
    SUBCASE("parse value")
    {
        const char* const argv[] = {"prog", "-name", "Alice"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(name == "Alice");
    }
}

TEST_CASE("CLIParser.Strings")
{
    CLIParser parser;
    std::vector<std::string> files;
    parser.add(&files, "file", "input files");

    SUBCASE("default is empty")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(files.empty());
    }
    SUBCASE("multiple values")
    {
        const char* const argv[] = {"prog", "-file", "a.txt", "-file", "b.txt", "-file", "c.txt"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(files.size() == 3);
        REQUIRE(files[0] == "a.txt");
        REQUIRE(files[1] == "b.txt");
        REQUIRE(files[2] == "c.txt");
    }
}

TEST_CASE("CLIParser.MultipleFlags")
{
    CLIParser parser;
    bool verbose = false;
    std::string output;
    i32 count = 0;
    parser.add(&verbose, "verbose", "verbose");
    parser.add(&output, "output", "output file");
    parser.add(&count, "count", "count");

    SUBCASE("all flags set")
    {
        const char* const argv[] = {"prog", "-verbose", "-output", "out.txt", "-count", "5"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(verbose == true);
        REQUIRE(output == "out.txt");
        REQUIRE(count == 5);
    }
    SUBCASE("with positional args")
    {
        const char* const argv[] = {"prog", "-verbose", "pos1", "-output", "out.txt", "pos2"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(verbose == true);
        REQUIRE(output == "out.txt");

        const auto args = parser.getArgs();
        REQUIRE(args.size() == 2);
        REQUIRE(args[0] == "pos1");
        REQUIRE(args[1] == "pos2");
    }
}

TEST_CASE("CLIParser.Errors")
{
    CLIParser parser;
    bool verbose = false;
    parser.add(&verbose, "verbose", "verbose");

    SUBCASE("unknown flag")
    {
        const char* const argv[] = {"prog", "--unknown"};
        auto err = parser.parse(argv);
        REQUIRE(err != nullptr);
        REQUIRE(err->toString().find("unknown flag") != std::string::npos);
    }
}

TEST_CASE("CLIParser.DoubleDash")
{
    CLIParser parser;
    bool verbose = false;
    parser.add(&verbose, "verbose", "verbose");

    SUBCASE("double dash separator")
    {
        const char* const argv[] = {"prog", "-verbose", "--", "--not-a-flag", "positional"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(verbose == true);

        const auto& args = parser.getArgs();
        REQUIRE(args.size() == 2);
        REQUIRE(args[0] == "--not-a-flag");
        REQUIRE(args[1] == "positional");
    }
}

TEST_CASE("CLIParser.NoArgs")
{
    CLIParser parser;

    SUBCASE("empty args")
    {
        const char* const argv[] = {"prog"};
        auto err = parser.parse(argv);
        REQUIRE(err == nullptr);
        REQUIRE(parser.getArgs().empty());
    }
}
