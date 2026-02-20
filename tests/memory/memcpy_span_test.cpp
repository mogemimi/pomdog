// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/memcpy_span.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::memcpySpan;

TEST_CASE("memcpySpan")
{
    SUBCASE("memcpySpan copies exact elements from source to destination")
    {
        std::array<i32, 5> src = {1, 2, 3, 4, 5};
        std::array<i32, 5> dst = {0, 0, 0, 0, 0};

        memcpySpan(std::span<i32>(dst), std::span<const i32>(src));

        CHECK(dst == src);
    }
    SUBCASE("memcpySpan copies partial range")
    {
        std::array<i32, 5> src = {10, 20, 30, 40, 50};
        std::array<i32, 5> dst = {0, 0, 0, 0, 0};

        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        memcpySpan(std::span<i32>(dst.begin(), 3), std::span<const i32>(src.begin(), 3));
        POMDOG_CLANG_UNSAFE_BUFFER_END

        CHECK(dst[0] == 10);
        CHECK(dst[1] == 20);
        CHECK(dst[2] == 30);
        CHECK(dst[3] == 0);
        CHECK(dst[4] == 0);
    }
    SUBCASE("memcpySpan throws if source is larger than destination")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::array<i32, 4> dst = {0, 0, 0, 0};
        std::array<i32, 5> src = {1, 2, 3, 4, 5};

        CHECK_THROWS_AS(memcpySpan(std::span<i32>(dst), std::span<const i32>(src)), std::out_of_range);
#endif
    }
    SUBCASE("memcpySpan works with std::vector")
    {
        std::vector<f32> src = {1.5f, 2.5f, 3.5f};
        std::vector<f32> dst(3);

        memcpySpan(std::span<f32>(dst), std::span<const f32>(src));

        CHECK(dst == src);
    }
    SUBCASE("dst and src spans with static extent size")
    {
        std::array<i32, 5> src = {1, 2, 3, 4, 5};
        std::array<i32, 5> dst = {0, 0, 0, 0, 0};

        memcpySpan(std::span<i32, 5>(dst), std::span<const i32, 5>(src));

        CHECK(dst == src);
    }
    SUBCASE("dst span with static extent size")
    {
        std::array<i32, 5> src = {1, 2, 3, 4, 5};
        std::array<i32, 5> dst = {0, 0, 0, 0, 0};

        memcpySpan(std::span<i32, 5>(dst), std::span<const i32>(src));

        CHECK(dst == src);
    }
    SUBCASE("src span with static extent size")
    {
        std::array<i32, 5> src = {1, 2, 3, 4, 5};
        std::array<i32, 5> dst = {0, 0, 0, 0, 0};

        memcpySpan(std::span<i32>(dst), std::span<const i32, 5>(src));

        CHECK(dst == src);
    }
}
