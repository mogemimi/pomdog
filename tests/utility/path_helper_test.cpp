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
using pomdog::filepaths::getBaseNameAsView;
using pomdog::filepaths::getDirectoryName;
using pomdog::filepaths::getDirectoryNameAsView;
using pomdog::filepaths::isAbsolute;
using pomdog::filepaths::isAbsoluteUnix;
using pomdog::filepaths::isAbsoluteWindows;
using pomdog::filepaths::join;
using pomdog::filepaths::joinUnix;
using pomdog::filepaths::joinWindows;
using pomdog::filepaths::normalize;
using pomdog::filepaths::normalizeUnix;
using pomdog::filepaths::normalizeWindows;
using pomdog::filepaths::relative;
using pomdog::filepaths::split;
using pomdog::filepaths::splitAsView;
using pomdog::filepaths::splitExtension;
using pomdog::filepaths::splitExtensionAsView;
using pomdog::filepaths::toSlash;

TEST_CASE("filepaths::joinWindows")
{
    REQUIRE(joinWindows("", "") == "");
    REQUIRE(joinWindows("a", "") == "a");
    REQUIRE(joinWindows("", "b") == "b");
    REQUIRE(joinWindows("a", "b") == "a\\b");
    REQUIRE(joinWindows("a/", "b") == "a\\b");
    REQUIRE(joinWindows("a", "/b") == "a\\b");
    REQUIRE(joinWindows("/a", "b") == "\\a\\b");
    REQUIRE(joinWindows("/a/", "b") == "\\a\\b");
    REQUIRE(joinWindows("a/b", "c/d") == "a\\b\\c\\d");
    REQUIRE(joinWindows("a", "b/c") == "a\\b\\c");
    REQUIRE(joinWindows("a/b", "c") == "a\\b\\c");
    REQUIRE(joinWindows("a/b", "/c") == "a\\b\\c");
    REQUIRE(joinWindows("a/b", "../xyz") == "a\\xyz");
    REQUIRE(joinWindows("a/b", "../../xyz") == "xyz");
    REQUIRE(joinWindows("a/b", "../../../xyz") == "..\\xyz");
}

TEST_CASE("filepaths::joinUnix")
{
    REQUIRE(joinUnix("", "") == "");
    REQUIRE(joinUnix("a", "") == "a");
    REQUIRE(joinUnix("", "b") == "b");
    REQUIRE(joinUnix("a", "b") == "a/b");
    REQUIRE(joinUnix("a/", "b") == "a/b");
    REQUIRE(joinUnix("a", "/b") == "a/b");
    REQUIRE(joinUnix("/a", "b") == "/a/b");
    REQUIRE(joinUnix("/a/", "b") == "/a/b");
    REQUIRE(joinUnix("a/b", "c/d") == "a/b/c/d");
    REQUIRE(joinUnix("a", "b/c") == "a/b/c");
    REQUIRE(joinUnix("a/b", "c") == "a/b/c");
    REQUIRE(joinUnix("a/b", "/c") == "a/b/c");
    REQUIRE(joinUnix("a/b", "../xyz") == "a/xyz");
    REQUIRE(joinUnix("a/b", "../../xyz") == "xyz");
    REQUIRE(joinUnix("a/b", "../../../xyz") == "../xyz");
}

TEST_CASE("filepaths::join")
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    REQUIRE(join("a", "b") == "a\\b");
    REQUIRE(join("a/b", "c") == "a\\b\\c");
#else
    REQUIRE(join("a", "b") == "a/b");
    REQUIRE(join("a/b", "c") == "a/b/c");
#endif
}

TEST_CASE("filepaths::getBaseName")
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

    // NOTE: Windows separators
    REQUIRE(getBaseName("foo\\bar.txt") == "bar.txt");
    REQUIRE(getBaseName("\\foo\\bar.txt") == "bar.txt");
    REQUIRE(getBaseName("\\foo\\bar\\baz.txt") == "baz.txt");
    REQUIRE(getBaseName("foo\\bar\\") == "");
    REQUIRE(getBaseName("C:\\foo\\bar.txt") == "bar.txt");

    // NOTE: Mixed separators
    REQUIRE(getBaseName("foo/bar\\baz.txt") == "baz.txt");
    REQUIRE(getBaseName("foo\\bar/baz.txt") == "baz.txt");
}

TEST_CASE("filepaths::getBaseNameAsView")
{
    REQUIRE(getBaseNameAsView("") == "");
    REQUIRE(getBaseNameAsView("foo") == "foo");
    REQUIRE(getBaseNameAsView("foo.txt") == "foo.txt");
    REQUIRE(getBaseNameAsView("/foo.txt") == "foo.txt");
    REQUIRE(getBaseNameAsView("foo/bar.txt") == "bar.txt");
    REQUIRE(getBaseNameAsView("/foo/bar.txt") == "bar.txt");
    REQUIRE(getBaseNameAsView("/foo/bar/baz.txt") == "baz.txt");
    REQUIRE(getBaseNameAsView("foo/bar/") == "");
    REQUIRE(getBaseNameAsView("foo bar/baz") == "baz");
    REQUIRE(getBaseNameAsView("a b/c d/e") == "e");
    REQUIRE(getBaseNameAsView("a/b c/d e/f") == "f");

    // NOTE: Windows separators
    REQUIRE(getBaseNameAsView("foo\\bar.txt") == "bar.txt");
    REQUIRE(getBaseNameAsView("\\foo\\bar.txt") == "bar.txt");
    REQUIRE(getBaseNameAsView("\\foo\\bar\\baz.txt") == "baz.txt");
    REQUIRE(getBaseNameAsView("foo\\bar\\") == "");
    REQUIRE(getBaseNameAsView("C:\\foo\\bar.txt") == "bar.txt");

    // NOTE: Mixed separators
    REQUIRE(getBaseNameAsView("foo/bar\\baz.txt") == "baz.txt");
    REQUIRE(getBaseNameAsView("foo\\bar/baz.txt") == "baz.txt");
}

TEST_CASE("filepaths::getDirectoryName")
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

    // NOTE: Windows separators
    REQUIRE(getDirectoryName("foo\\bar.txt") == "foo");
    REQUIRE(getDirectoryName("\\foo\\bar.txt") == "\\foo");
    REQUIRE(getDirectoryName("\\foo\\bar\\baz.txt") == "\\foo\\bar");
    REQUIRE(getDirectoryName("foo\\bar\\") == "foo\\bar");
    REQUIRE(getDirectoryName("\\foo.txt") == "\\");
    REQUIRE(getDirectoryName("\\") == "\\");
    REQUIRE(getDirectoryName("C:\\foo\\bar.txt") == "C:\\foo");

    // NOTE: Mixed separators
    REQUIRE(getDirectoryName("foo/bar\\baz.txt") == "foo/bar");
    REQUIRE(getDirectoryName("foo\\bar/baz.txt") == "foo\\bar");
}

TEST_CASE("filepaths::getDirectoryNameAsView")
{
    REQUIRE(getDirectoryNameAsView("") == "");
    REQUIRE(getDirectoryNameAsView("foo") == "");
    REQUIRE(getDirectoryNameAsView("foo.txt") == "");
    REQUIRE(getDirectoryNameAsView("foo/bar.txt") == "foo");
    REQUIRE(getDirectoryNameAsView("/foo/bar.txt") == "/foo");
    REQUIRE(getDirectoryNameAsView("/foo/bar/baz.txt") == "/foo/bar");
    REQUIRE(getDirectoryNameAsView("a b/c") == "a b");
    REQUIRE(getDirectoryNameAsView("a b/c d/e") == "a b/c d");
    REQUIRE(getDirectoryNameAsView("a/b c/d e/f") == "a/b c/d e");
    REQUIRE(getDirectoryNameAsView("/foo.txt") == "/");
    REQUIRE(getDirectoryNameAsView("foo/bar/") == "foo/bar");
    REQUIRE(getDirectoryNameAsView("/foo/bar/baz") == "/foo/bar");
    REQUIRE(getDirectoryNameAsView("/foo/bar/baz/") == "/foo/bar/baz");
    REQUIRE(getDirectoryNameAsView("../foo.txt") == "..");
    REQUIRE(getDirectoryNameAsView("..") == "");
    REQUIRE(getDirectoryNameAsView(".") == "");
    REQUIRE(getDirectoryNameAsView("/") == "/");

    // NOTE: Windows separators
    REQUIRE(getDirectoryNameAsView("foo\\bar.txt") == "foo");
    REQUIRE(getDirectoryNameAsView("\\foo\\bar.txt") == "\\foo");
    REQUIRE(getDirectoryNameAsView("\\foo\\bar\\baz.txt") == "\\foo\\bar");
    REQUIRE(getDirectoryNameAsView("foo\\bar\\") == "foo\\bar");
    REQUIRE(getDirectoryNameAsView("\\foo.txt") == "\\");
    REQUIRE(getDirectoryNameAsView("\\") == "\\");
    REQUIRE(getDirectoryNameAsView("C:\\foo\\bar.txt") == "C:\\foo");

    // NOTE: Mixed separators
    REQUIRE(getDirectoryNameAsView("foo/bar\\baz.txt") == "foo/bar");
    REQUIRE(getDirectoryNameAsView("foo\\bar/baz.txt") == "foo\\bar");
}

TEST_CASE("filepaths::splitAsView")
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

    // NOTE: Windows separators
    result = split("foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = split("\\foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "\\foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = split("foo\\bar\\baz.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "baz.txt");

    // NOTE: Mixed separators
    result = split("foo/bar\\baz.txt");
    REQUIRE(std::get<0>(result) == "foo/bar");
    REQUIRE(std::get<1>(result) == "baz.txt");

    result = split("foo\\bar/baz.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "baz.txt");
}

TEST_CASE("filepaths::splitAsView")
{
    auto result = splitAsView("");
    REQUIRE(std::get<0>(result) == "");
    REQUIRE(std::get<1>(result) == "");

    result = splitAsView("foo");
    REQUIRE(std::get<0>(result) == "");
    REQUIRE(std::get<1>(result) == "foo");

    result = splitAsView("foo/bar.txt");
    REQUIRE(std::get<0>(result) == "foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = splitAsView("/foo/bar.txt");
    REQUIRE(std::get<0>(result) == "/foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = splitAsView("foo/bar/baz.txt");
    REQUIRE(std::get<0>(result) == "foo/bar");
    REQUIRE(std::get<1>(result) == "baz.txt");

    result = splitAsView("a b/c d/e.txt");
    REQUIRE(std::get<0>(result) == "a b/c d");
    REQUIRE(std::get<1>(result) == "e.txt");

    result = splitAsView("a/b c/d e/f.txt");
    REQUIRE(std::get<0>(result) == "a/b c/d e");
    REQUIRE(std::get<1>(result) == "f.txt");

    // NOTE: Windows separators
    result = splitAsView("foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = splitAsView("\\foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "\\foo");
    REQUIRE(std::get<1>(result) == "bar.txt");

    result = splitAsView("foo\\bar\\baz.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "baz.txt");

    // NOTE: Mixed separators
    result = splitAsView("foo/bar\\baz.txt");
    REQUIRE(std::get<0>(result) == "foo/bar");
    REQUIRE(std::get<1>(result) == "baz.txt");

    result = splitAsView("foo\\bar/baz.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "baz.txt");
}

TEST_CASE("filepaths::splitExtension")
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
    REQUIRE(std::get<1>(result) == "txt");

    // NOTE: Windows separators (extension splitting is separator-independent)
    result = splitExtension("foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "txt");

    result = splitExtension("\\foo\\bar.baz.txt");
    REQUIRE(std::get<0>(result) == "\\foo\\bar.baz");
    REQUIRE(std::get<1>(result) == "txt");
}

TEST_CASE("filepaths::splitExtensionAsView")
{
    auto result = splitExtensionAsView("");
    REQUIRE(std::get<0>(result) == "");
    REQUIRE(std::get<1>(result) == "");

    result = splitExtensionAsView("foo");
    REQUIRE(std::get<0>(result) == "foo");
    REQUIRE(std::get<1>(result) == "");

    result = splitExtensionAsView("foo.txt");
    REQUIRE(std::get<0>(result) == "foo");
    REQUIRE(std::get<1>(result) == "txt");

    result = splitExtensionAsView("foo.bar.txt");
    REQUIRE(std::get<0>(result) == "foo.bar");
    REQUIRE(std::get<1>(result) == "txt");

    result = splitExtensionAsView("/foo/bar.txt");
    REQUIRE(std::get<0>(result) == "/foo/bar");
    REQUIRE(std::get<1>(result) == "txt");

    result = splitExtensionAsView("foo/bar.baz.txt");
    REQUIRE(std::get<0>(result) == "foo/bar.baz");
    REQUIRE(std::get<1>(result) == "txt");

    // NOTE: Windows separators (extension splitting is separator-independent)
    result = splitExtensionAsView("foo\\bar.txt");
    REQUIRE(std::get<0>(result) == "foo\\bar");
    REQUIRE(std::get<1>(result) == "txt");

    result = splitExtensionAsView("\\foo\\bar.baz.txt");
    REQUIRE(std::get<0>(result) == "\\foo\\bar.baz");
    REQUIRE(std::get<1>(result) == "txt");
}

TEST_CASE("filepaths::normalizeWindows")
{
    REQUIRE(normalizeWindows("") == ".");
    REQUIRE(normalizeWindows(".") == ".");
    REQUIRE(normalizeWindows("./") == ".");
    REQUIRE(normalizeWindows("./.") == ".");
    REQUIRE(normalizeWindows("././") == ".");
    REQUIRE(normalizeWindows("././.") == ".");
    REQUIRE(normalizeWindows("./..") == "..");
    REQUIRE(normalizeWindows("..") == "..");
    REQUIRE(normalizeWindows("./foo") == "foo");

    REQUIRE(normalizeWindows("../..") == "..\\..");
    REQUIRE(normalizeWindows("../../..") == "..\\..\\..");
    REQUIRE(normalizeWindows("../.") == "..");
    REQUIRE(normalizeWindows(".././..") == "..\\..");
    REQUIRE(normalizeWindows("/.") == "\\");
    REQUIRE(normalizeWindows("/..") == "\\");
    REQUIRE(normalizeWindows("/usr/local/bin") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/bin/") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/bin/.") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/bin/./") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/./bin") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/./local/bin") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/bin/..") == "\\usr\\local");
    REQUIRE(normalizeWindows("/usr/local/bin/../") == "\\usr\\local");
    REQUIRE(normalizeWindows("/usr/local/bin/../..") == "\\usr");
    REQUIRE(normalizeWindows("/usr/local/bin/../../") == "\\usr");
    REQUIRE(normalizeWindows("/usr/local/../local/bin") == "\\usr\\local\\bin");
    REQUIRE(normalizeWindows("/usr/local/../local/bin/..") == "\\usr\\local");

    REQUIRE(normalizeWindows("foo/./bar") == "foo\\bar");
    REQUIRE(normalizeWindows("foo/../bar") == "bar");
    REQUIRE(normalizeWindows("foo/bar/..") == "foo");
    REQUIRE(normalizeWindows("/foo/bar/..") == "\\foo");
    REQUIRE(normalizeWindows("/foo/bar/../baz") == "\\foo\\baz");
}

TEST_CASE("filepaths::normalizeUnix")
{
    REQUIRE(normalizeUnix("") == ".");
    REQUIRE(normalizeUnix(".") == ".");
    REQUIRE(normalizeUnix("./") == ".");
    REQUIRE(normalizeUnix("./.") == ".");
    REQUIRE(normalizeUnix("././") == ".");
    REQUIRE(normalizeUnix("././.") == ".");
    REQUIRE(normalizeUnix("./..") == "..");
    REQUIRE(normalizeUnix("..") == "..");
    REQUIRE(normalizeUnix("./foo") == "foo");

    REQUIRE(normalizeUnix("../..") == "../..");
    REQUIRE(normalizeUnix("../../..") == "../../..");
    REQUIRE(normalizeUnix("../.") == "..");
    REQUIRE(normalizeUnix(".././..") == "../..");
    REQUIRE(normalizeUnix("/.") == "/");
    REQUIRE(normalizeUnix("/..") == "/");
    REQUIRE(normalizeUnix("/usr/local/bin") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/bin/") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/bin/.") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/bin/./") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/./bin") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/./local/bin") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/bin/..") == "/usr/local");
    REQUIRE(normalizeUnix("/usr/local/bin/../") == "/usr/local");
    REQUIRE(normalizeUnix("/usr/local/bin/../..") == "/usr");
    REQUIRE(normalizeUnix("/usr/local/bin/../../") == "/usr");
    REQUIRE(normalizeUnix("/usr/local/../local/bin") == "/usr/local/bin");
    REQUIRE(normalizeUnix("/usr/local/../local/bin/..") == "/usr/local");

    REQUIRE(normalizeUnix("foo/./bar") == "foo/bar");
    REQUIRE(normalizeUnix("foo/../bar") == "bar");
    REQUIRE(normalizeUnix("foo/bar/..") == "foo");
    REQUIRE(normalizeUnix("/foo/bar/..") == "/foo");
    REQUIRE(normalizeUnix("/foo/bar/../baz") == "/foo/baz");
}

TEST_CASE("filepaths::normalize")
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    REQUIRE(normalize("") == ".");
    REQUIRE(normalize("./foo") == "foo");
    REQUIRE(normalize("/usr/local/bin") == "\\usr\\local\\bin");
    REQUIRE(normalize("foo/./bar") == "foo\\bar");
#else
    REQUIRE(normalize("") == ".");
    REQUIRE(normalize("./foo") == "foo");
    REQUIRE(normalize("/usr/local/bin") == "/usr/local/bin");
    REQUIRE(normalize("foo/./bar") == "foo/bar");
#endif
}

TEST_CASE("filepaths::relative")
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

TEST_CASE("filepaths::isAbsoluteWindows")
{
    REQUIRE(isAbsoluteWindows("C:/foo"));
    REQUIRE(isAbsoluteWindows("C:\\foo"));
    REQUIRE(isAbsoluteWindows("D:/foo/bar"));
    REQUIRE(isAbsoluteWindows("C:\\"));
    REQUIRE(isAbsoluteWindows("D:\\"));
    REQUIRE(isAbsoluteWindows("C:\\a"));
    REQUIRE(isAbsoluteWindows("C:\\a\\"));
    REQUIRE(isAbsoluteWindows("\\\\a"));
    REQUIRE(isAbsoluteWindows("\\\\a\\"));
    REQUIRE(isAbsoluteWindows("\\\\a\\b"));
    REQUIRE(isAbsoluteWindows("//"));
    REQUIRE(isAbsoluteWindows("//a"));
    REQUIRE(isAbsoluteWindows("//a/b"));
    REQUIRE(isAbsoluteWindows("///"));
    REQUIRE(isAbsoluteWindows("////"));
    REQUIRE(isAbsoluteWindows("/////"));
    REQUIRE(isAbsoluteWindows("a:/xyz"));

    REQUIRE_FALSE(isAbsoluteWindows("\\a"));
    REQUIRE_FALSE(isAbsoluteWindows("/"));
    REQUIRE_FALSE(isAbsoluteWindows("/a/"));
    REQUIRE_FALSE(isAbsoluteWindows("/a/b"));
    REQUIRE_FALSE(isAbsoluteWindows("/abc"));
    REQUIRE_FALSE(isAbsoluteWindows("/abc/"));
    REQUIRE_FALSE(isAbsoluteWindows("ab:/xyz"));
    REQUIRE_FALSE(isAbsoluteWindows("abc:/xyz"));

    REQUIRE_FALSE(isAbsoluteWindows(".."));
    REQUIRE_FALSE(isAbsoluteWindows("../"));
    REQUIRE_FALSE(isAbsoluteWindows("../a"));
    REQUIRE_FALSE(isAbsoluteWindows("../a/"));
    REQUIRE_FALSE(isAbsoluteWindows("a"));
    REQUIRE_FALSE(isAbsoluteWindows(""));
    REQUIRE_FALSE(isAbsoluteWindows("."));
    REQUIRE_FALSE(isAbsoluteWindows("./"));
    REQUIRE_FALSE(isAbsoluteWindows("./a"));
    REQUIRE_FALSE(isAbsoluteWindows("./a/"));
    REQUIRE_FALSE(isAbsoluteWindows("./a/b"));
    REQUIRE_FALSE(isAbsoluteWindows("foo"));
    REQUIRE_FALSE(isAbsoluteWindows("foo/bar"));
    REQUIRE_FALSE(isAbsoluteWindows("..\\"));
    REQUIRE_FALSE(isAbsoluteWindows("..\\a"));
    REQUIRE_FALSE(isAbsoluteWindows("..\\a\\"));
    REQUIRE_FALSE(isAbsoluteWindows("..\\a\\b"));
    REQUIRE_FALSE(isAbsoluteWindows(".\\"));
    REQUIRE_FALSE(isAbsoluteWindows(".\\a"));
    REQUIRE_FALSE(isAbsoluteWindows(".\\a\\"));
    REQUIRE_FALSE(isAbsoluteWindows(".\\a\\b"));
}
TEST_CASE("filepaths::isAbsoluteUnix")
{
    REQUIRE(isAbsoluteUnix("/foo"));
    REQUIRE(isAbsoluteUnix("/foo/bar"));
    REQUIRE(isAbsoluteUnix("/"));
    REQUIRE(isAbsoluteUnix("/a/"));
    REQUIRE(isAbsoluteUnix("/a/b"));
    REQUIRE(isAbsoluteUnix("/abc"));
    REQUIRE(isAbsoluteUnix("/abc/"));

    REQUIRE_FALSE(isAbsoluteUnix(".."));
    REQUIRE_FALSE(isAbsoluteUnix("../"));
    REQUIRE_FALSE(isAbsoluteUnix("../a"));
    REQUIRE_FALSE(isAbsoluteUnix("../a/"));
    REQUIRE_FALSE(isAbsoluteUnix("a"));
    REQUIRE_FALSE(isAbsoluteUnix(""));
    REQUIRE_FALSE(isAbsoluteUnix("."));
    REQUIRE_FALSE(isAbsoluteUnix("./"));
    REQUIRE_FALSE(isAbsoluteUnix("./a"));
    REQUIRE_FALSE(isAbsoluteUnix("./a/"));
    REQUIRE_FALSE(isAbsoluteUnix("./a/b"));
    REQUIRE_FALSE(isAbsoluteUnix("foo"));
    REQUIRE_FALSE(isAbsoluteUnix("foo/bar"));
}
TEST_CASE("filepaths::isAbsolute")
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    REQUIRE(isAbsolute("\\\\foo"));
    REQUIRE(isAbsolute("C:/foo"));
    REQUIRE_FALSE(isAbsolute("foo"));
#else
    REQUIRE(isAbsolute("/foo"));
    REQUIRE_FALSE(isAbsolute("foo"));
#endif
}
TEST_CASE("filepaths::toSlash")
{
    REQUIRE(toSlash("") == "");
    REQUIRE(toSlash("\\") == "/");
    REQUIRE(toSlash("\\\\") == "//");
    REQUIRE(toSlash("\\a\\b") == "/a/b");
    REQUIRE(toSlash("\\a\\b\\") == "/a/b/");
    REQUIRE(toSlash("a\\b") == "a/b");
    REQUIRE(toSlash("a\\b\\") == "a/b/");
}
