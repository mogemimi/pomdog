// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/aligned_alloc.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

TEST_CASE("aligned_alloc")
{
    SUBCASE("basic allocation and alignment")
    {
        constexpr size_t alignment = 64;
        constexpr size_t size = 256;

        void* ptr = pomdog::memory::aligned_alloc(alignment, size);
        REQUIRE(ptr != nullptr);
        REQUIRE(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        // NOTE: fill memory to ensure it's usable
        POMDOG_CLANG_SUPPRESS_WARNING_PUSH
        POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-container")
        std::span<std::byte> buffer{reinterpret_cast<std::byte*>(ptr), size};
        POMDOG_CLANG_SUPPRESS_WARNING_POP

        std::fill(buffer.begin(), buffer.end(), std::byte(0xAA));

        pomdog::memory::aligned_free(ptr);
    }
    SUBCASE("different alignments")
    {
        const std::array<size_t, 6> arr = {8, 16, 32, 64, 128, 256};

        for (size_t alignment : arr) {
            void* ptr = pomdog::memory::aligned_alloc(alignment, 1024);
            CHECK(ptr != nullptr);
            CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
            pomdog::memory::aligned_free(ptr);
        }
    }
    SUBCASE("size equal to alignment")
    {
        const std::array<size_t, 6> arr = {8, 16, 32, 64, 128, 256};

        for (size_t x : arr) {
            const size_t alignment = x;
            const size_t size = x;
            void* ptr = pomdog::memory::aligned_alloc(alignment, size);
            CHECK(ptr != nullptr);
            CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
            pomdog::memory::aligned_free(ptr);
        }
    }
    SUBCASE("alignment not power of two (should fail or be handled)")
    {
        constexpr size_t alignment = 30; // not a power of 2
        constexpr size_t size = 256;

        void* ptr = pomdog::memory::aligned_alloc(alignment, size);
        REQUIRE(ptr == nullptr);

        if (ptr != nullptr) {
            pomdog::memory::aligned_free(ptr);
        }
    }
}
