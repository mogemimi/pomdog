// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/linear_page_allocator.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::memory::LinearPageAllocator;

TEST_CASE("LinearPageAllocator")
{
    SUBCASE("default construct")
    {
        LinearPageAllocator allocator;
        allocator.reset(256);
    }
    SUBCASE("allocate")
    {
        LinearPageAllocator allocator;
        allocator.reset(256);

        const auto ptr1 = allocator.allocate(16, alignof(int));
        REQUIRE(ptr1 != nullptr);

        const auto ptr2 = allocator.allocate(32, alignof(int));
        REQUIRE(ptr2 != nullptr);
        REQUIRE(ptr2 != ptr1);
    }
    SUBCASE("allocate aligned")
    {
        LinearPageAllocator allocator;
        allocator.reset(256);

        const auto ptr = allocator.allocate(16, 16);
        REQUIRE(ptr != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % 16 == 0);
    }
    SUBCASE("reset")
    {
        LinearPageAllocator allocator;
        allocator.reset(256);

        REQUIRE(allocator.allocate(64, alignof(int)) != nullptr);
        REQUIRE(allocator.allocate(64, alignof(int)) != nullptr);

        allocator.reset();

        // NOTE: After reset, we should be able to allocate again
        const auto ptr = allocator.allocate(64, alignof(int));
        REQUIRE(ptr != nullptr);
    }
    SUBCASE("allocate zero size")
    {
        LinearPageAllocator allocator;
        REQUIRE(allocator.allocate(0, 0) == nullptr);
        REQUIRE(allocator.allocate(0, 1) == nullptr);
        REQUIRE(allocator.allocate(0, 2) == nullptr);

        REQUIRE(allocator.allocate(1, 0) == nullptr);
        REQUIRE(allocator.allocate(1, 3) == nullptr);
        REQUIRE(allocator.allocate(1, 7) == nullptr);
    }
    SUBCASE("allocate larger than page")
    {
        // NOTE: LinearPageAllocator returns nullptr for allocations larger than page size
        LinearPageAllocator allocator;
        allocator.reset(64);

        const auto ptr = allocator.allocate(128, alignof(int));
        REQUIRE(ptr == nullptr);
    }
    SUBCASE("allocate multiple pages")
    {
        LinearPageAllocator allocator;
        allocator.reset(64);

        for (int i = 0; i < 16; ++i) {
            const auto ptr = allocator.allocate(32, alignof(int));
            REQUIRE(ptr != nullptr);
        }
    }
    SUBCASE("allocate and use memory")
    {
        LinearPageAllocator allocator;
        const auto p = allocator.allocate(1, 1);
        REQUIRE(p != nullptr);

        std::memset(p, 'A', 1);
        REQUIRE(std::memcmp(p, "A", 1) == 0);
    }
    SUBCASE("allocate and use memory 2")
    {
        LinearPageAllocator allocator;
        const auto p = reinterpret_cast<char*>(allocator.allocate(4, 1));
        REQUIRE(p != nullptr);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        std::memset(p, '#', 3);
        REQUIRE(std::memcmp(p, "###D", 3) == 0);
    }
    SUBCASE("allocate and use memory 3")
    {
        LinearPageAllocator allocator;
        const auto p = reinterpret_cast<char*>(allocator.allocate(4, 1));
        REQUIRE(p != nullptr);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        const auto q = reinterpret_cast<char*>(allocator.allocate(7, 1));
        REQUIRE(q != nullptr);
        REQUIRE(p != q);

        std::memcpy(q, "1234567", 7);
        REQUIRE(std::memcmp(q, "1234567", 7) == 0);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);
    }
    SUBCASE("allocate many small blocks")
    {
        LinearPageAllocator allocator;
        for (int i = 0; i < 512; i++) {
            const auto p = reinterpret_cast<char*>(allocator.allocate(127, 1));
            REQUIRE(p != nullptr);
        }
        allocator.reset();

        for (int i = 0; i < 512; i++) {
            const auto p = reinterpret_cast<char*>(allocator.allocate(64, 1));
            REQUIRE(p != nullptr);
        }
    }
}
