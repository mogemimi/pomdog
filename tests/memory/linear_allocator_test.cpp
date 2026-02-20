// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/linear_allocator.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <span>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::memory::LinearAllocator;

TEST_CASE("LinearAllocator")
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN

    SUBCASE("default construct")
    {
        LinearAllocator allocator;
        REQUIRE(allocator.getAllocatedSize() == 0);
        REQUIRE(allocator.getBufferSize() == 0);

        std::array<std::uint8_t, 256> buffer{};
        allocator.reset(buffer);
        REQUIRE(allocator.getBufferSize() == buffer.size());
        REQUIRE(allocator.getAllocatedSize() == 0);
    }
    SUBCASE("allocate")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        auto ptr1 = allocator.allocate(16, alignof(int));
        REQUIRE(ptr1 != nullptr);
        REQUIRE(allocator.getAllocatedSize() >= 16);

        auto ptr2 = allocator.allocate(32, alignof(int));
        REQUIRE(ptr2 != nullptr);
        REQUIRE(ptr2 != ptr1);
        REQUIRE(allocator.getAllocatedSize() >= 48);
    }
    SUBCASE("allocate aligned")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        auto ptr = allocator.allocate(16, 16);
        REQUIRE(ptr != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % 16 == 0);
    }
    SUBCASE("reset")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        (void)allocator.allocate(64, alignof(int));
        REQUIRE(allocator.getAllocatedSize() >= 64);

        allocator.reset();
        REQUIRE(allocator.getAllocatedSize() == 0);
        REQUIRE(allocator.getBufferSize() == 0);
    }
    SUBCASE("allocate overflow")
    {
        std::array<std::uint8_t, 64> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        auto ptr = allocator.allocate(128, alignof(int));
        REQUIRE(ptr == nullptr);
    }
    SUBCASE("allocate multiple")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        for (int i = 0; i < 8; ++i) {
            auto ptr = allocator.allocate(16, alignof(int));
            REQUIRE(ptr != nullptr);
        }

        REQUIRE(allocator.getAllocatedSize() >= 128);
    }
    SUBCASE("isOwnerOf")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        auto ptr = allocator.allocate(16, alignof(int));
        REQUIRE(ptr != nullptr);
        REQUIRE(allocator.isOwnerOf(ptr));

        std::uint8_t external = 42;
        REQUIRE_FALSE(allocator.isOwnerOf(&external));
    }
    SUBCASE("allocate zero size")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        REQUIRE(allocator.allocate(0, 0) == nullptr);
        REQUIRE(allocator.allocate(0, 1) == nullptr);
        REQUIRE(allocator.allocate(0, 2) == nullptr);

        REQUIRE(allocator.allocate(1, 0) == nullptr);
        REQUIRE(allocator.allocate(1, 3) == nullptr);
        REQUIRE(allocator.allocate(1, 7) == nullptr);
    }
    SUBCASE("basic usage")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        const auto p = allocator.allocate(1, 1);
        REQUIRE(p != nullptr);
        REQUIRE(allocator.isOwnerOf(p));
        REQUIRE(allocator.getAllocatedSize() == 1);
        REQUIRE(allocator.getBufferSize() >= 1);

        std::memset(p, 'A', 1);
        REQUIRE(std::memcmp(p, "A", 1) == 0);
    }
    SUBCASE("allocate and use memory")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        const auto p = reinterpret_cast<char*>(allocator.allocate(4, 1));
        REQUIRE(p != nullptr);
        REQUIRE(allocator.isOwnerOf(p));
        REQUIRE(allocator.getAllocatedSize() == 4);
        REQUIRE(allocator.getBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        std::memset(p, '#', 3);
        REQUIRE(std::memcmp(p, "###D", 3) == 0);
    }
    SUBCASE("allocate multiple and use memory")
    {
        std::array<std::uint8_t, 256> buffer{};
        LinearAllocator allocator;
        allocator.reset(buffer);

        const auto p = reinterpret_cast<char*>(allocator.allocate(4, 1));
        REQUIRE(p != nullptr);
        REQUIRE(allocator.isOwnerOf(p));
        REQUIRE(allocator.getAllocatedSize() == 4);
        REQUIRE(allocator.getBufferSize() >= 4);

        std::memcpy(p, "ABCD", 4);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);

        REQUIRE(allocator.getAllocatedSize() == 4);
        REQUIRE(allocator.getBufferSize() >= 4);

        const auto q = reinterpret_cast<char*>(allocator.allocate(7, 1));
        REQUIRE(q != nullptr);
        REQUIRE(p != q);
        REQUIRE(allocator.isOwnerOf(q));
        REQUIRE(allocator.getAllocatedSize() == 4 + 7);
        REQUIRE(allocator.getBufferSize() >= 4 + 7);

        std::memcpy(q, "1234567", 7);
        REQUIRE(std::memcmp(q, "1234567", 7) == 0);
        REQUIRE(std::memcmp(p, "ABCD", 4) == 0);
    }

    POMDOG_CLANG_UNSAFE_BUFFER_END
}
