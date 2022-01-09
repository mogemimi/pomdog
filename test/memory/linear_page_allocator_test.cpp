// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_page_allocator.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

using namespace pomdog;

TEST_CASE("linear_page_allocator", "[memory]")
{
    using pomdog::detail::LinearPageAllocator;
    LinearPageAllocator alloc;

    SECTION("")
    {
        //REQUIRE(alloc.GetAllocatedSize() == 0);
        //REQUIRE(alloc.GetBufferSize() == 0);
        alloc.Reset();
        //REQUIRE(alloc.GetAllocatedSize() == 0);
        //REQUIRE(alloc.GetBufferSize() == 0);
    }
    SECTION("")
    {
        REQUIRE(alloc.Allocate(0, 0) == nullptr);
        REQUIRE(alloc.Allocate(0, 1) == nullptr);
        REQUIRE(alloc.Allocate(0, 2) == nullptr);

        REQUIRE(alloc.Allocate(1, 0) == nullptr);
        REQUIRE(alloc.Allocate(1, 3) == nullptr);
        REQUIRE(alloc.Allocate(1, 7) == nullptr);
    }
    SECTION("")
    {
        const auto p = alloc.Allocate(1, 1);
        REQUIRE(p != nullptr);
        //REQUIRE(alloc.IsOwnerOf(p));
        //REQUIRE(alloc.GetAllocatedSize() == 1);
        //REQUIRE(alloc.GetBufferSize() >= 1);

        std::memset(p, 'A', 1);
        REQUIRE(std::memcmp(p, "A", 1) == 0);
    }
    SECTION("")
    {
        const auto p = reinterpret_cast<char*>(alloc.Allocate(4, 1));
        REQUIRE(p != nullptr);
        //REQUIRE(alloc.IsOwnerOf(p));
        //REQUIRE(alloc.GetAllocatedSize() == 4);
        //REQUIRE(alloc.GetBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        std::memset(p, '#', 3);
        REQUIRE(std::memcmp(p, "###D", 3) == 0);
    }
    SECTION("")
    {
        const auto p = reinterpret_cast<char*>(alloc.Allocate(4, 1));
        REQUIRE(p != nullptr);
        //REQUIRE(alloc.IsOwnerOf(p));
        //REQUIRE(alloc.GetAllocatedSize() == 4);
        //REQUIRE(alloc.GetBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        //REQUIRE(alloc.GetAllocatedSize() == 4);
        //REQUIRE(alloc.GetBufferSize() >= 4);

        const auto q = reinterpret_cast<char*>(alloc.Allocate(7, 1));
        REQUIRE(q != nullptr);
        REQUIRE(p != q);
        //REQUIRE(alloc.IsOwnerOf(q));
        //REQUIRE(alloc.GetAllocatedSize() == 4 + 7);
        //REQUIRE(alloc.GetBufferSize() >= 4 + 7);

        std::memcpy(q, "1234567", 7);
        REQUIRE(std::memcmp(q, "1234567", 7) == 0);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);
    }
    SECTION("")
    {
        for (int i = 0; i < 512; i++) {
            const auto p = reinterpret_cast<char*>(alloc.Allocate(127, 1));
            REQUIRE(p != nullptr);
        }
        alloc.Reset();

        for (int i = 0; i < 512; i++) {
            const auto p = reinterpret_cast<char*>(alloc.Allocate(64, 1));
            REQUIRE(p != nullptr);
        }
    }
}
