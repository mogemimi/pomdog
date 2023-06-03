// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/basic/platform.h"
#include "pomdog/utility/path_helper.h"
#include <catch_amalgamated.hpp>

namespace filepaths = pomdog::filepaths;

TEST_CASE("filepaths", "[filepaths]")
{
    SECTION("join")
    {
        REQUIRE(filepaths::join("", "") == "");
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(filepaths::join("Chirico", "Cuvie") == "Chirico\\Cuvie");
        REQUIRE(filepaths::join("", "Cuvie") == "Cuvie");
        REQUIRE(filepaths::join("Chirico", "") == "Chirico");
        REQUIRE(filepaths::join("PS/Chirico", "Cuvie") == "PS/Chirico\\Cuvie");
        REQUIRE(filepaths::join("Chirico", "Cuvie/AT") == "Chirico\\Cuvie/AT");
        REQUIRE(filepaths::join("PS/Chirico", "Cuvie/AT") == "PS/Chirico\\Cuvie/AT");
#else
        REQUIRE(filepaths::join("Chirico", "Cuvie") == "Chirico/Cuvie");
        REQUIRE(filepaths::join("", "Cuvie") == "Cuvie");
        REQUIRE(filepaths::join("Chirico", "") == "Chirico");
        REQUIRE(filepaths::join("PS/Chirico", "Cuvie") == "PS/Chirico/Cuvie");
        REQUIRE(filepaths::join("Chirico", "Cuvie/AT") == "Chirico/Cuvie/AT");
        REQUIRE(filepaths::join("PS/Chirico", "Cuvie/AT") == "PS/Chirico/Cuvie/AT");
#endif
    }
    SECTION("getBaseName")
    {
        REQUIRE(filepaths::getBaseName("Fiana") == "Fiana");
        REQUIRE(filepaths::getBaseName("Phantam Lady/Fiana") == "Fiana");
        REQUIRE(filepaths::getBaseName("Proto One/Phantam Lady/Fiana") == "Fiana");
        REQUIRE(filepaths::getBaseName("PS/Proto One/Phantam Lady/Fiana") == "Fiana");
    }
    SECTION("getDirectoryName")
    {
        REQUIRE(filepaths::getDirectoryName("Fiana") == "");
        REQUIRE(filepaths::getDirectoryName("Phantam Lady/Fiana") == "Phantam Lady");
        REQUIRE(filepaths::getDirectoryName("Proto One/Phantam Lady/Fiana") == "Proto One/Phantam Lady");
        REQUIRE(filepaths::getDirectoryName("PS/Proto One/Phantam Lady/Fiana") == "PS/Proto One/Phantam Lady");
    }
    SECTION("split")
    {
        auto result = filepaths::split("Fiana");
        REQUIRE(std::get<0>(result) == "");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::split("Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::split("Proto One/Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::split("PS/Proto One/Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::split("PS/Proto One/Phantam Lady/Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana.sotai");
    }
    SECTION("splitExtension")
    {
        auto result = filepaths::splitExtension("Fiana");
        REQUIRE(std::get<0>(result) == "Fiana");
        REQUIRE(std::get<1>(result) == "");

        result = filepaths::splitExtension("Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::splitExtension("Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::splitExtension("PS/Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = filepaths::splitExtension("PS/Proto One/Phantam Lady.Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<1>(result) == "sotai");

        result = filepaths::splitExtension("PS.Proto One/Phantam Lady.Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS.Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<1>(result) == "sotai");
    }
    SECTION("normalize")
    {
        const auto normalize = filepaths::normalize;
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        // NOTE: Windows
        REQUIRE(normalize("") == "");
        REQUIRE(normalize(".") == ".");
        REQUIRE(normalize("./") == ".");
        REQUIRE(normalize("./.") == ".");
        REQUIRE(normalize("././") == ".");
        REQUIRE(normalize("././.") == ".");
        REQUIRE(normalize("./..") == "..");
        REQUIRE(normalize("..") == "..");
        REQUIRE(normalize("../..") == "..\\..");
        REQUIRE(normalize("../../..") == "..\\..\\..");
        REQUIRE(normalize("../.") == "..");
        REQUIRE(normalize(".././..") == "..\\..");
        REQUIRE(normalize("/.") == "/");
        REQUIRE(normalize("/..") == "/");
        REQUIRE(normalize("/usr/local/bin") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/bin/") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/bin/.") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/bin/./") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/./bin") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/./local/bin") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/bin/..") == "/usr\\local");
        REQUIRE(normalize("/usr/local/bin/../") == "/usr\\local");
        REQUIRE(normalize("/usr/local/bin/../..") == "/usr");
        REQUIRE(normalize("/usr/local/bin/../../") == "/usr");
        REQUIRE(normalize("/usr/local/../local/bin") == "/usr\\local\\bin");
        REQUIRE(normalize("/usr/local/../local/bin/..") == "/usr\\local");
#else
        // NOTE: Unix
        REQUIRE(normalize("") == "");
        REQUIRE(normalize(".") == ".");
        REQUIRE(normalize("./") == ".");
        REQUIRE(normalize("./.") == ".");
        REQUIRE(normalize("././") == ".");
        REQUIRE(normalize("././.") == ".");
        REQUIRE(normalize("./..") == "..");
        REQUIRE(normalize("..") == "..");
        REQUIRE(normalize("../..") == "../..");
        REQUIRE(normalize("../../..") == "../../..");
        REQUIRE(normalize("../.") == "..");
        REQUIRE(normalize(".././..") == "../..");
        REQUIRE(normalize("/.") == "/");
        REQUIRE(normalize("/..") == "/");
        REQUIRE(normalize("/usr/local/bin") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/bin/") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/bin/.") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/bin/./") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/./bin") == "/usr/local/bin");
        REQUIRE(normalize("/usr/./local/bin") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/bin/..") == "/usr/local");
        REQUIRE(normalize("/usr/local/bin/../") == "/usr/local");
        REQUIRE(normalize("/usr/local/bin/../..") == "/usr");
        REQUIRE(normalize("/usr/local/bin/../../") == "/usr");
        REQUIRE(normalize("/usr/local/../local/bin") == "/usr/local/bin");
        REQUIRE(normalize("/usr/local/../local/bin/..") == "/usr/local");
#endif
    }
    SECTION("relative")
    {
        const auto relative = filepaths::relative;
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(relative("/usr/local/bin", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/.", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/./", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/././", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/./././", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/..", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/../", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/.././", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/./../", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/../../", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/bin/../..", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/bin/../../.", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/bin/.././../", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/bin/./../../", "/usr/local/bin") == "..\\..");

        REQUIRE(relative("/usr/local/../local/bin", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/..", "/usr/local/bin") == "..\\..");
        REQUIRE(relative("/usr/local/../local/..", "/usr/local/bin") == "..\\..");

        REQUIRE(relative("/usr/local/bin/gcc", "/usr/local/bin") == "gcc");
        REQUIRE(relative("/usr/local/opt", "/usr/local/bin") == "..\\opt");
        REQUIRE(relative("/usr/share", "/usr/local/bin") == "..\\..\\share");
        REQUIRE(relative("/usr/share/dict", "/usr/local/bin") == "..\\..\\share\\dict");
#else
        REQUIRE(relative("/usr/local/bin", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/.", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/./", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/././", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/./././", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local/bin/..", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/../", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/.././", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/./../", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr/local/bin/../../", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/bin/../..", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/bin/../../.", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/bin/.././../", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/bin/./../../", "/usr/local/bin") == "../..");

        REQUIRE(relative("/usr/local/../local/bin", "/usr/local/bin") == ".");
        REQUIRE(relative("/usr/local", "/usr/local/bin") == "..");
        REQUIRE(relative("/usr", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/..", "/usr/local/bin") == "../..");
        REQUIRE(relative("/usr/local/../local/..", "/usr/local/bin") == "../..");

        REQUIRE(relative("/usr/local/bin/gcc", "/usr/local/bin") == "gcc");
        REQUIRE(relative("/usr/local/opt", "/usr/local/bin") == "../opt");
        REQUIRE(relative("/usr/share", "/usr/local/bin") == "../../share");
        REQUIRE(relative("/usr/share/dict", "/usr/local/bin") == "../../share/dict");
#endif
    }
    SECTION("isAbsolute")
    {
        REQUIRE(filepaths::isAbsolute("/"));
        REQUIRE(filepaths::isAbsolute("/a/"));
        REQUIRE(filepaths::isAbsolute("/a/b"));
        REQUIRE(filepaths::isAbsolute("/abc"));
        REQUIRE(filepaths::isAbsolute("/abc/"));
        REQUIRE(!filepaths::isAbsolute(".."));
        REQUIRE(!filepaths::isAbsolute("../"));
        REQUIRE(!filepaths::isAbsolute("../a"));
        REQUIRE(!filepaths::isAbsolute("../a/"));
        REQUIRE(!filepaths::isAbsolute("a"));
        REQUIRE(!filepaths::isAbsolute(""));
        REQUIRE(!filepaths::isAbsolute("."));
        REQUIRE(!filepaths::isAbsolute("./"));
        REQUIRE(!filepaths::isAbsolute("./a"));
        REQUIRE(!filepaths::isAbsolute("./a/"));
        REQUIRE(!filepaths::isAbsolute("./a/b"));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(filepaths::isAbsolute("C:\\"));
        REQUIRE(filepaths::isAbsolute("D:\\"));
        REQUIRE(filepaths::isAbsolute("C:\\a"));
        REQUIRE(filepaths::isAbsolute("C:\\a\\"));
        REQUIRE(filepaths::isAbsolute("\\\\a"));
        REQUIRE(filepaths::isAbsolute("\\\\a\\"));
        REQUIRE(filepaths::isAbsolute("\\\\a\\b"));
        REQUIRE(!filepaths::isAbsolute("..\\"));
        REQUIRE(!filepaths::isAbsolute("..\\a"));
        REQUIRE(!filepaths::isAbsolute("..\\a\\"));
        REQUIRE(!filepaths::isAbsolute("..\\a\\b"));
        REQUIRE(!filepaths::isAbsolute(".\\"));
        REQUIRE(!filepaths::isAbsolute(".\\a"));
        REQUIRE(!filepaths::isAbsolute(".\\a\\"));
        REQUIRE(!filepaths::isAbsolute(".\\a\\b"));
#endif
    }
    SECTION("toSlash")
    {
        REQUIRE(filepaths::toSlash("") == "");
        REQUIRE(filepaths::toSlash("\\") == "/");
        REQUIRE(filepaths::toSlash("\\\\") == "//");
        REQUIRE(filepaths::toSlash("\\a\\b") == "/a/b");
        REQUIRE(filepaths::toSlash("\\a\\b\\") == "/a/b/");
        REQUIRE(filepaths::toSlash("a\\b") == "a/b");
        REQUIRE(filepaths::toSlash("a\\b\\") == "a/b/");
    }
}
