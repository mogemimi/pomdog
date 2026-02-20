// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/compilation_target_architecture.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/aligned_alloc.h"
#include "pomdog/utility/bit_cast_span.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstdlib>
#include <new>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::f64;
using pomdog::i16;
using pomdog::i32;
using pomdog::i8;
using pomdog::toByteArraySpan;
using pomdog::u32;
using pomdog::u64;
using pomdog::u8;

TEST_CASE("toByteArraySpan")
{
    SUBCASE("f64 -> u8")
    {
        std::vector<f64> values = {1.23, 4.56};
        std::span<f64> src = values;

        std::span<u8> dst = toByteArraySpan<u8>(src);

        CHECK(dst.size() == sizeof(f64) * values.size());
        CHECK(reinterpret_cast<void*>(dst.data()) == reinterpret_cast<void*>(src.data()));
    }
    SUBCASE("vector: f32 -> u8")
    {
        std::vector<f32> values = {1.0f, 2.0f, 3.0f};
        std::span<f32> src = values;

        std::span<u8> dst = toByteArraySpan<u8>(src);

        CHECK(dst.size() == sizeof(f32) * values.size());
        CHECK(reinterpret_cast<void*>(dst.data()) == reinterpret_cast<void*>(src.data()));
    }
    SUBCASE("array: f32 -> u8")
    {
        std::array<f32, 2> values = {1.0f, -2.0f};
        std::span<f32> src = values;

        std::span<u8> dst = toByteArraySpan<u8>(src);

        CHECK(dst.size() == sizeof(f32) * values.size());
        CHECK(reinterpret_cast<void*>(dst.data()) == reinterpret_cast<void*>(src.data()));
    }
    SUBCASE("array: i32 -> i8")
    {
        std::array<i32, 1> src = {0x12345678};
        std::span<i8> dst = toByteArraySpan<i8>(src);

        REQUIRE(dst.size() == 4);

#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
        CHECK(dst[0] == 0x78);
        CHECK(dst[1] == 0x56);
        CHECK(dst[2] == 0x34);
        CHECK(dst[3] == 0x12);
#endif
    }
    SUBCASE("const array: u32 -> u8")
    {
        const std::array<u32, 2> src = {u32(0x12345678), u32(0xCAFE98D0)};
        std::span<const u8> dst = toByteArraySpan<const u8>(src);

        REQUIRE(dst.size() == 8);

#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
        CHECK(dst[0] == u8(0x78));
        CHECK(dst[1] == u8(0x56));
        CHECK(dst[2] == u8(0x34));
        CHECK(dst[3] == u8(0x12));
        CHECK(dst[4] == u8(0xD0));
        CHECK(dst[5] == u8(0x98));
        CHECK(dst[6] == u8(0xFE));
        CHECK(dst[7] == u8(0xCA));
#endif
    }
    SUBCASE("toByteArraySpan supports const data")
    {
        const std::array<u32, 2> arr = {0xAAAAAAAA, 0x55555555};
        std::span<const u32> src = arr;

        auto dst = toByteArraySpan<const u8>(src);
        CHECK(dst.size() == sizeof(u32) * 2);
        static_assert(std::is_same_v<decltype(dst), std::span<const u8>>);
    }
    SUBCASE("toByteArraySpan handles empty input")
    {
        std::span<i16> emptySpan;
        auto byteSpan = toByteArraySpan<u8>(emptySpan);

        CHECK(byteSpan.size() == 0);
        CHECK(byteSpan.data() == nullptr);
    }
    SUBCASE("enum Foo -> u8")
    {
        enum class Foo : u8 {
            A = 0,
            B = 1,
            C = 2
        };

        std::vector<Foo> values = {Foo::A, Foo::B, Foo::C};
        std::span<Foo> src = values;

        std::span<u8> dst = toByteArraySpan<u8>(src);

        CHECK(dst.size() == values.size());
        CHECK(dst[0] == static_cast<u8>(Foo::A));
        CHECK(dst[1] == static_cast<u8>(Foo::B));
        CHECK(dst[2] == static_cast<u8>(Foo::C));
    }
    SUBCASE("alignment - from aligned f64 buffer")
    {
        alignas(f64) std::array<f64, 2> values = {3.14, 2.718};
        std::span<f64> span = values;

        auto byteSpan = toByteArraySpan<u8>(span);

        CHECK(reinterpret_cast<void*>(byteSpan.data()) == reinterpret_cast<void*>(span.data()));

        const std::uintptr_t ptrValue = reinterpret_cast<std::uintptr_t>(byteSpan.data());
        CHECK((ptrValue % alignof(f64)) == 0);
    }
    SUBCASE("alignment - from minimally aligned type")
    {
        std::array<u8, 4> bytes = {1, 2, 3, 4};
        std::span<u8> span = bytes;

        auto byteSpan = toByteArraySpan<u8>(span);

        CHECK(reinterpret_cast<void*>(byteSpan.data()) == reinterpret_cast<void*>(span.data()));

        const std::uintptr_t ptrValue = reinterpret_cast<std::uintptr_t>(byteSpan.data());
        CHECK((ptrValue % alignof(u8)) == 0);
    }
    SUBCASE("alignment - custom aligned struct")
    {
        struct alignas(32) Aligned32 {
            u64 data[4];
        };
        static_assert(alignof(Aligned32) == 32);
        static_assert(sizeof(Aligned32) == 32);

        alignas(32) std::array<Aligned32, 1> buffer;
        std::span<Aligned32> span = buffer;

        auto byteSpan = toByteArraySpan<u8>(span);

        CHECK(reinterpret_cast<void*>(byteSpan.data()) == reinterpret_cast<void*>(span.data()));
        CHECK(byteSpan.size() == sizeof(Aligned32));

        const std::uintptr_t ptrValue = reinterpret_cast<std::uintptr_t>(byteSpan.data());
        CHECK((ptrValue % alignof(Aligned32)) == 0);
    }
    SUBCASE("toByteArraySpan alignment - using aligned memory pool")
    {
        struct alignas(64) Aligned64 {
            u64 data[8];
        };
        static_assert(alignof(Aligned64) == 64);
        static_assert(sizeof(Aligned64) == 64);

        constexpr std::size_t elementCount = 4;
        constexpr std::size_t alignment = alignof(Aligned64);
        constexpr std::size_t size = sizeof(Aligned64) * elementCount;

        void* rawMem = pomdog::memory::aligned_alloc(alignment, size);
        REQUIRE(rawMem != nullptr);

        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        std::span<Aligned64> span{reinterpret_cast<Aligned64*>(rawMem), elementCount};
        POMDOG_CLANG_UNSAFE_BUFFER_END

        auto byteSpan = toByteArraySpan<u8>(span);

        CHECK(reinterpret_cast<void*>(byteSpan.data()) == reinterpret_cast<void*>(span.data()));
        CHECK(byteSpan.size() == sizeof(Aligned64) * elementCount);

        const std::uintptr_t ptrValue = reinterpret_cast<std::uintptr_t>(byteSpan.data());
        CHECK((ptrValue % alignment) == 0);

        pomdog::memory::aligned_free(rawMem);
    }
}
