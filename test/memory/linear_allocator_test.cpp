// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/placement_new.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

#include <chrono>
#include <iostream>

using namespace pomdog;

TEST_CASE("LinearAllocator", "[memory]")
{
    pomdog::memory::LinearAllocator alloc;

    SECTION("")
    {
        REQUIRE(alloc.getAllocatedSize() == 0);
        REQUIRE(alloc.getBufferSize() == 0);
        alloc.reset();
        REQUIRE(alloc.getAllocatedSize() == 0);
        REQUIRE(alloc.getBufferSize() == 0);
    }
    SECTION("")
    {
        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        REQUIRE(alloc.allocate(0, 0) == nullptr);
        REQUIRE(alloc.allocate(0, 1) == nullptr);
        REQUIRE(alloc.allocate(0, 2) == nullptr);

        REQUIRE(alloc.allocate(1, 0) == nullptr);
        REQUIRE(alloc.allocate(1, 3) == nullptr);
        REQUIRE(alloc.allocate(1, 7) == nullptr);
    }
    SECTION("")
    {
        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        const auto p = alloc.allocate(1, 1);
        REQUIRE(p != nullptr);
        REQUIRE(alloc.isOwnerOf(p));
        REQUIRE(alloc.getAllocatedSize() == 1);
        REQUIRE(alloc.getBufferSize() >= 1);

        std::memset(p, 'A', 1);
        REQUIRE(std::memcmp(p, "A", 1) == 0);
    }
    SECTION("")
    {
        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        const auto p = reinterpret_cast<char*>(alloc.allocate(4, 1));
        REQUIRE(p != nullptr);
        REQUIRE(alloc.isOwnerOf(p));
        REQUIRE(alloc.getAllocatedSize() == 4);
        REQUIRE(alloc.getBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        std::memset(p, '#', 3);
        REQUIRE(std::memcmp(p, "###D", 3) == 0);
    }
    SECTION("")
    {
        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        const auto p = reinterpret_cast<char*>(alloc.allocate(4, 1));
        REQUIRE(p != nullptr);
        REQUIRE(alloc.isOwnerOf(p));
        REQUIRE(alloc.getAllocatedSize() == 4);
        REQUIRE(alloc.getBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        REQUIRE(alloc.getAllocatedSize() == 4);
        REQUIRE(alloc.getBufferSize() >= 4);

        const auto q = reinterpret_cast<char*>(alloc.allocate(7, 1));
        REQUIRE(q != nullptr);
        REQUIRE(p != q);
        REQUIRE(alloc.isOwnerOf(q));
        REQUIRE(alloc.getAllocatedSize() == 4 + 7);
        REQUIRE(alloc.getBufferSize() >= 4 + 7);

        std::memcpy(q, "1234567", 7);
        REQUIRE(std::memcmp(q, "1234567", 7) == 0);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);
    }
}
