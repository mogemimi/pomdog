// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_compiletime.h"
#include "pomdog/utility/xxhash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u32;
using pomdog::detail::strlen_compiletime;
using pomdog::hash::xxh32;

TEST_CASE("xxhash32")
{
    {
        constexpr auto input = "";
        static_assert(xxh32(input, strlen_compiletime(input), u32(0)) == u32(46947589UL));
        static_assert(xxh32(input, strlen_compiletime(input), u32(20160723UL)) == u32(3775933984UL));
    }
    {
        constexpr auto input = "Hello";
        static_assert(xxh32(input, strlen_compiletime(input), u32(0)) == u32(4060533391UL));
        static_assert(xxh32(input, strlen_compiletime(input), u32(20160723UL)) == u32(2645664716UL));
    }
    {
        constexpr auto input = "abcdefghijklmnopqrstuvwxyz";
        static_assert(xxh32(input, strlen_compiletime(input), u32(0)) == u32(1671515487UL));
        static_assert(xxh32(input, strlen_compiletime(input), u32(20160723UL)) == u32(2306886178UL));
    }
    {
        constexpr auto input = "Extremely fast non-cryptographic hash algorithm";
        static_assert(xxh32(input, strlen_compiletime(input), u32(0)) == u32(177718476UL));
        static_assert(xxh32(input, strlen_compiletime(input), u32(20160723UL)) == u32(1002827396UL));
    }
}
