
// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/bit_cast_span.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::memsetSpan;
using pomdog::toByteArraySpan;
using pomdog::u8;

TEST_CASE("memsetSpan")
{
    SUBCASE("memsetSpan sets all bytes to zero for u8")
    {
        std::array<u8, 5> data = {1, 2, 3, 4, 5};
        memsetSpan(std::span<u8>(data), 0);

        for (auto val : data) {
            CHECK(val == 0);
        }
    }
    SUBCASE("memsetSpan fills all bytes with 0xFF for u8")
    {
        std::array<u8, 4> data = {0, 0, 0, 0};
        memsetSpan(std::span<u8>(data), 0xFF);

        for (auto val : data) {
            CHECK(val == 0xFF);
        }
    }
    SUBCASE("memsetSpan sets all bytes to zero for int array")
    {
        std::array<i32, 4> data = {1, 2, 3, 4};
        memsetSpan(toByteArraySpan<u8>(data), 0);

        for (i32 val : data) {
            CHECK(val == 0);
        }
    }
    SUBCASE("memsetSpan sets all bytes to zero for float array")
    {
        std::array<f32, 3> data = {1.0f, -2.0f, 3.5f};
        memsetSpan(toByteArraySpan<u8>(data), 0);

        for (f32 val : data) {
            CHECK(val == 0.0f);
        }
    }
    SUBCASE("memsetSpan fills raw buffer correctly")
    {
        std::array<char, 8> buf;
        memsetSpan(std::span<char>(buf), 'A');

        for (char c : buf) {
            CHECK(c == 'A');
        }
    }
}
