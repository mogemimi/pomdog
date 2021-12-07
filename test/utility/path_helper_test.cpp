// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/basic/platform.hpp"
#include "pomdog/utility/path_helper.hpp"
#include <catch_amalgamated.hpp>

namespace PathHelper = Pomdog::PathHelper;

TEST_CASE("PathHelper", "[PathHelper]")
{
    SECTION("Join")
    {
        REQUIRE(PathHelper::Join("", "") == "");
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(PathHelper::Join("Chirico", "Cuvie") == "Chirico\\Cuvie");
        REQUIRE(PathHelper::Join("", "Cuvie") == "Cuvie");
        REQUIRE(PathHelper::Join("Chirico", "") == "Chirico");
        REQUIRE(PathHelper::Join("PS/Chirico", "Cuvie") == "PS/Chirico\\Cuvie");
        REQUIRE(PathHelper::Join("Chirico", "Cuvie/AT") == "Chirico\\Cuvie/AT");
        REQUIRE(PathHelper::Join("PS/Chirico", "Cuvie/AT") == "PS/Chirico\\Cuvie/AT");
#else
        REQUIRE(PathHelper::Join("Chirico", "Cuvie") == "Chirico/Cuvie");
        REQUIRE(PathHelper::Join("", "Cuvie") == "Cuvie");
        REQUIRE(PathHelper::Join("Chirico", "") == "Chirico");
        REQUIRE(PathHelper::Join("PS/Chirico", "Cuvie") == "PS/Chirico/Cuvie");
        REQUIRE(PathHelper::Join("Chirico", "Cuvie/AT") == "Chirico/Cuvie/AT");
        REQUIRE(PathHelper::Join("PS/Chirico", "Cuvie/AT") == "PS/Chirico/Cuvie/AT");
#endif
    }
    SECTION("GetBaseName")
    {
        REQUIRE(PathHelper::GetBaseName("Fiana") == "Fiana");
        REQUIRE(PathHelper::GetBaseName("Phantam Lady/Fiana") == "Fiana");
        REQUIRE(PathHelper::GetBaseName("Proto One/Phantam Lady/Fiana") == "Fiana");
        REQUIRE(PathHelper::GetBaseName("PS/Proto One/Phantam Lady/Fiana") == "Fiana");
    }
    SECTION("GetDirectoryName")
    {
        REQUIRE(PathHelper::GetDirectoryName("Fiana") == "");
        REQUIRE(PathHelper::GetDirectoryName("Phantam Lady/Fiana") == "Phantam Lady");
        REQUIRE(PathHelper::GetDirectoryName("Proto One/Phantam Lady/Fiana") == "Proto One/Phantam Lady");
        REQUIRE(PathHelper::GetDirectoryName("PS/Proto One/Phantam Lady/Fiana") == "PS/Proto One/Phantam Lady");
    }
    SECTION("Split")
    {
        auto result = PathHelper::Split("Fiana");
        REQUIRE(std::get<0>(result) == "");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::Split("Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::Split("Proto One/Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::Split("PS/Proto One/Phantam Lady/Fiana");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::Split("PS/Proto One/Phantam Lady/Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana.sotai");
    }
    SECTION("SplitExtension")
    {
        auto result = PathHelper::SplitExtension("Fiana");
        REQUIRE(std::get<0>(result) == "Fiana");
        REQUIRE(std::get<1>(result) == "");

        result = PathHelper::SplitExtension("Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::SplitExtension("Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::SplitExtension("PS/Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady");
        REQUIRE(std::get<1>(result) == "Fiana");

        result = PathHelper::SplitExtension("PS/Proto One/Phantam Lady.Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS/Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<1>(result) == "sotai");

        result = PathHelper::SplitExtension("PS.Proto One/Phantam Lady.Fiana.sotai");
        REQUIRE(std::get<0>(result) == "PS.Proto One/Phantam Lady.Fiana");
        REQUIRE(std::get<1>(result) == "sotai");
    }
    SECTION("Normalize")
    {
        const auto normalize = PathHelper::Normalize;
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
    SECTION("Relative")
    {
        const auto relative = PathHelper::Relative;
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
    SECTION("IsAbsolute")
    {
        REQUIRE(PathHelper::IsAbsolute("/"));
        REQUIRE(PathHelper::IsAbsolute("/a/"));
        REQUIRE(PathHelper::IsAbsolute("/a/b"));
        REQUIRE(PathHelper::IsAbsolute("/abc"));
        REQUIRE(PathHelper::IsAbsolute("/abc/"));
        REQUIRE(!PathHelper::IsAbsolute(".."));
        REQUIRE(!PathHelper::IsAbsolute("../"));
        REQUIRE(!PathHelper::IsAbsolute("../a"));
        REQUIRE(!PathHelper::IsAbsolute("../a/"));
        REQUIRE(!PathHelper::IsAbsolute("a"));
        REQUIRE(!PathHelper::IsAbsolute(""));
        REQUIRE(!PathHelper::IsAbsolute("."));
        REQUIRE(!PathHelper::IsAbsolute("./"));
        REQUIRE(!PathHelper::IsAbsolute("./a"));
        REQUIRE(!PathHelper::IsAbsolute("./a/"));
        REQUIRE(!PathHelper::IsAbsolute("./a/b"));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(PathHelper::IsAbsolute("C:\\"));
        REQUIRE(PathHelper::IsAbsolute("D:\\"));
        REQUIRE(PathHelper::IsAbsolute("C:\\a"));
        REQUIRE(PathHelper::IsAbsolute("C:\\a\\"));
        REQUIRE(PathHelper::IsAbsolute("\\\\a"));
        REQUIRE(PathHelper::IsAbsolute("\\\\a\\"));
        REQUIRE(PathHelper::IsAbsolute("\\\\a\\b"));
        REQUIRE(!PathHelper::IsAbsolute("..\\"));
        REQUIRE(!PathHelper::IsAbsolute("..\\a"));
        REQUIRE(!PathHelper::IsAbsolute("..\\a\\"));
        REQUIRE(!PathHelper::IsAbsolute("..\\a\\b"));
        REQUIRE(!PathHelper::IsAbsolute(".\\"));
        REQUIRE(!PathHelper::IsAbsolute(".\\a"));
        REQUIRE(!PathHelper::IsAbsolute(".\\a\\"));
        REQUIRE(!PathHelper::IsAbsolute(".\\a\\b"));
#endif
    }
    SECTION("ToSlash")
    {
        REQUIRE(PathHelper::ToSlash("") == "");
        REQUIRE(PathHelper::ToSlash("\\") == "/");
        REQUIRE(PathHelper::ToSlash("\\\\") == "//");
        REQUIRE(PathHelper::ToSlash("\\a\\b") == "/a/b");
        REQUIRE(PathHelper::ToSlash("\\a\\b\\") == "/a/b/");
        REQUIRE(PathHelper::ToSlash("a\\b") == "a/b");
        REQUIRE(PathHelper::ToSlash("a\\b\\") == "a/b/");
    }
}
