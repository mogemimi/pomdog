// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_compiletime.h"
#include "pomdog/utility/xxhash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u64;
using pomdog::detail::strlen_compiletime;
using pomdog::hash::xxh64;

TEST_CASE("xxhash64")
{
    {
        constexpr auto input = "";
        static_assert(xxh64(input, strlen_compiletime(input), u64(0)) == u64(17241709254077376921ULL));
        static_assert(xxh64(input, strlen_compiletime(input), u64(20160723ULL)) == u64(10525815183674050875ULL));
    }
    {
        constexpr auto input = "Hello";
        static_assert(xxh64(input, strlen_compiletime(input), u64(0)) == u64(753694413698530628ULL));
        static_assert(xxh64(input, strlen_compiletime(input), u64(20160723ULL)) == u64(3119102237357052223ULL));
    }
    {
        constexpr auto input = "abcdefghijklmnopqrstuvwxyz";
        static_assert(xxh64(input, strlen_compiletime(input), u64(0)) == u64(14979520437024293724ULL));
        static_assert(xxh64(input, strlen_compiletime(input), u64(20160723ULL)) == u64(10755511906999168518ULL));
    }
    {
        constexpr auto input = "Extremely fast non-cryptographic hash algorithm";
        static_assert(xxh64(input, strlen_compiletime(input), u64(0)) == u64(5301103970522645570ULL));
        static_assert(xxh64(input, strlen_compiletime(input), u64(20160723ULL)) == u64(2118451276975851972ULL));
    }
}
