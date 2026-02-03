// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::filepaths::getBaseName;
using pomdog::filepaths::getDirectoryName;
using pomdog::filepaths::isAbsolute;
using pomdog::filepaths::join;
using pomdog::filepaths::normalize;
using pomdog::filepaths::relative;
using pomdog::filepaths::split;
using pomdog::filepaths::splitExtension;
using pomdog::filepaths::toSlash;

TEST_CASE("PathHelper")
{
    SUBCASE("join")
    {
        REQUIRE(join("", "") == "");
        REQUIRE(join("a", "") == "a");
        REQUIRE(join("", "b") == "b");
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(join("a", "b") == "a\\b");
        REQUIRE(join("a/", "b") == "a/b");
        REQUIRE(join("a", "/b") == "a/b");
        REQUIRE(join("/a", "b") == "/a\\b");
        REQUIRE(join("/a/", "b") == "/a/b");
        REQUIRE(join("a/b", "c/d") == "a/b\\c/d");
        REQUIRE(join("a", "b/c") == "a\\b/c");
        REQUIRE(join("a/b", "c") == "a/b\\c");
        REQUIRE(join("a/b", "/c") == "a/b/c");
        REQUIRE(join("a/b", "../../../xyz") == "a/b\\../../../xyz");
#else
        REQUIRE(join("a", "b") == "a/b");
        REQUIRE(join("a/", "b") == "a/b");
        REQUIRE(join("a", "/b") == "a/b");
        REQUIRE(join("/a", "b") == "/a/b");
        REQUIRE(join("/a/", "b") == "/a/b");
        REQUIRE(join("a/b", "c/d") == "a/b/c/d");
        REQUIRE(join("a", "b/c") == "a/b/c");
        REQUIRE(join("a/b", "c") == "a/b/c");
        REQUIRE(join("a/b", "/c") == "a/b/c");
        REQUIRE(join("a/b", "../../../xyz") == "a/b/../../../xyz");
#endif
    }
    SUBCASE("getBaseName")
    {
        REQUIRE(getBaseName("") == "");
        REQUIRE(getBaseName("foo") == "foo");
        REQUIRE(getBaseName("foo.txt") == "foo.txt");
        REQUIRE(getBaseName("/foo.txt") == "foo.txt");
        REQUIRE(getBaseName("foo/bar.txt") == "bar.txt");
        REQUIRE(getBaseName("/foo/bar.txt") == "bar.txt");
        REQUIRE(getBaseName("/foo/bar/baz.txt") == "baz.txt");
        REQUIRE(getBaseName("foo/bar/") == "");
        REQUIRE(getBaseName("foo bar/baz") == "baz");
        REQUIRE(getBaseName("a b/c d/e") == "e");
        REQUIRE(getBaseName("a/b c/d e/f") == "f");
    }
    SUBCASE("getDirectoryName")
    {
        REQUIRE(getDirectoryName("") == "");
        REQUIRE(getDirectoryName("foo") == "");
        REQUIRE(getDirectoryName("foo.txt") == "");
        REQUIRE(getDirectoryName("foo/bar.txt") == "foo");
        REQUIRE(getDirectoryName("/foo/bar.txt") == "/foo");
        REQUIRE(getDirectoryName("/foo/bar/baz.txt") == "/foo/bar");
        REQUIRE(getDirectoryName("a b/c") == "a b");
        REQUIRE(getDirectoryName("a b/c d/e") == "a b/c d");
        REQUIRE(getDirectoryName("a/b c/d e/f") == "a/b c/d e");
        REQUIRE(getDirectoryName("/foo.txt") == "/");
        REQUIRE(getDirectoryName("foo/bar/") == "foo/bar");
        REQUIRE(getDirectoryName("/foo/bar/baz.txt") == "/foo/bar");
        REQUIRE(getDirectoryName("/foo/bar/baz") == "/foo/bar");
        REQUIRE(getDirectoryName("/foo/bar/baz/") == "/foo/bar/baz");
        REQUIRE(getDirectoryName("../foo.txt") == "..");
        REQUIRE(getDirectoryName("..") == "");
        REQUIRE(getDirectoryName(".") == "");
        REQUIRE(getDirectoryName("/") == "/");
    }
    SUBCASE("split")
    {
        auto result = split("");
        REQUIRE(std::get<0>(result) == "");
        REQUIRE(std::get<1>(result) == "");

        result = split("foo");
        REQUIRE(std::get<0>(result) == "");
        REQUIRE(std::get<1>(result) == "foo");

        result = split("foo/bar.txt");
        REQUIRE(std::get<0>(result) == "foo");
        REQUIRE(std::get<1>(result) == "bar.txt");

        result = split("/foo/bar.txt");
        REQUIRE(std::get<0>(result) == "/foo");
        REQUIRE(std::get<1>(result) == "bar.txt");

        result = split("foo/bar/baz.txt");
        REQUIRE(std::get<0>(result) == "foo/bar");
        REQUIRE(std::get<1>(result) == "baz.txt");

        result = split("a b/c d/e.txt");
        REQUIRE(std::get<0>(result) == "a b/c d");
        REQUIRE(std::get<1>(result) == "e.txt");

        result = split("a/b c/d e/f.txt");
        REQUIRE(std::get<0>(result) == "a/b c/d e");
        REQUIRE(std::get<1>(result) == "f.txt");
    }
    SUBCASE("splitExtension")
    {
        auto result = splitExtension("");
        REQUIRE(std::get<0>(result) == "");
        REQUIRE(std::get<1>(result) == "");

        result = splitExtension("foo");
        REQUIRE(std::get<0>(result) == "foo");
        REQUIRE(std::get<1>(result) == "");

        result = splitExtension("foo.txt");
        REQUIRE(std::get<0>(result) == "foo");
        REQUIRE(std::get<1>(result) == "txt");

        result = splitExtension("foo.bar.txt");
        REQUIRE(std::get<0>(result) == "foo.bar");
        REQUIRE(std::get<1>(result) == "txt");

        result = splitExtension("/foo/bar.txt");
        REQUIRE(std::get<0>(result) == "/foo/bar");
        REQUIRE(std::get<1>(result) == "txt");

        result = splitExtension("foo/bar.baz.txt");
        REQUIRE(std::get<0>(result) == "foo/bar.baz");
    }
    SUBCASE("normalize")
    {
        REQUIRE(normalize("") == "");
        REQUIRE(normalize(".") == ".");
        REQUIRE(normalize("./") == ".");
        REQUIRE(normalize("./.") == ".");
        REQUIRE(normalize("././") == ".");
        REQUIRE(normalize("././.") == ".");
        REQUIRE(normalize("./..") == "..");
        REQUIRE(normalize("..") == "..");
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        // NOTE: Windows
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

        REQUIRE(normalize("./foo") == ".\\foo");
        REQUIRE(normalize("foo/./bar") == "foo\\bar");
        REQUIRE(normalize("foo/../bar") == "foo\\..\\bar");
        REQUIRE(normalize("foo/bar/..") == "foo");

        // NOTE: Windows uses backslash as path separator but preserves leading slash
        REQUIRE(normalize("/foo/bar/..") == "/foo");
        REQUIRE(normalize("/foo/bar/../baz") == "/foo\\baz");
#else
        // NOTE: Unix
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

        REQUIRE(normalize("./foo") == "foo");
        REQUIRE(normalize("foo/./bar") == "foo/bar");
        REQUIRE(normalize("foo/../bar") == "bar");
        REQUIRE(normalize("foo/bar/..") == "foo");
        REQUIRE(normalize("/foo/bar/..") == "/foo");
        REQUIRE(normalize("/foo/bar/../baz") == "/foo/baz");
#endif
    }
    SUBCASE("relative")
    {
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
    SUBCASE("isAbsolute")
    {
        REQUIRE(isAbsolute("/foo"));
        REQUIRE(isAbsolute("/foo/bar"));
        REQUIRE(isAbsolute("/"));
        REQUIRE(isAbsolute("/a/"));
        REQUIRE(isAbsolute("/a/b"));
        REQUIRE(isAbsolute("/abc"));
        REQUIRE(isAbsolute("/abc/"));
        REQUIRE_FALSE(isAbsolute(".."));
        REQUIRE_FALSE(isAbsolute("../"));
        REQUIRE_FALSE(isAbsolute("../a"));
        REQUIRE_FALSE(isAbsolute("../a/"));
        REQUIRE_FALSE(isAbsolute("a"));
        REQUIRE_FALSE(isAbsolute(""));
        REQUIRE_FALSE(isAbsolute("."));
        REQUIRE_FALSE(isAbsolute("./"));
        REQUIRE_FALSE(isAbsolute("./a"));
        REQUIRE_FALSE(isAbsolute("./a/"));
        REQUIRE_FALSE(isAbsolute("./a/b"));
        REQUIRE_FALSE(isAbsolute("foo"));
        REQUIRE_FALSE(isAbsolute("foo/bar"));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(isAbsolute("C:/foo"));
        REQUIRE(isAbsolute("C:\\foo"));
        REQUIRE(isAbsolute("D:/foo/bar"));
        REQUIRE(isAbsolute("C:\\"));
        REQUIRE(isAbsolute("D:\\"));
        REQUIRE(isAbsolute("C:\\a"));
        REQUIRE(isAbsolute("C:\\a\\"));
        REQUIRE(isAbsolute("\\\\a"));
        REQUIRE(isAbsolute("\\\\a\\"));
        REQUIRE(isAbsolute("\\\\a\\b"));
        REQUIRE_FALSE(isAbsolute("..\\"));
        REQUIRE_FALSE(isAbsolute("..\\a"));
        REQUIRE_FALSE(isAbsolute("..\\a\\"));
        REQUIRE_FALSE(isAbsolute("..\\a\\b"));
        REQUIRE_FALSE(isAbsolute(".\\"));
        REQUIRE_FALSE(isAbsolute(".\\a"));
        REQUIRE_FALSE(isAbsolute(".\\a\\"));
        REQUIRE_FALSE(isAbsolute(".\\a\\b"));
#endif
    }
    SUBCASE("toSlash")
    {
        REQUIRE(toSlash("") == "");
        REQUIRE(toSlash("\\") == "/");
        REQUIRE(toSlash("\\\\") == "//");
        REQUIRE(toSlash("\\a\\b") == "/a/b");
        REQUIRE(toSlash("\\a\\b\\") == "/a/b/");
        REQUIRE(toSlash("a\\b") == "a/b");
        REQUIRE(toSlash("a\\b\\") == "a/b/");
    }
}
