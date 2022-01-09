// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/placement_new.h"
#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/linear_page_allocator.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

using namespace pomdog;

TEST_CASE("placement_new", "[memory]")
{
    struct alignas(16) Foo final {
        std::int32_t X = 0;
        std::int32_t Y = 0;
        std::int32_t Z = 0;
        std::int32_t W = 0;
    };

    SECTION("LinearAllocator")
    {
        using pomdog::detail::LinearAllocator;
        LinearAllocator alloc;

        std::array<std::uint8_t, 1024> buffer;
        alloc.Reset(buffer);

        auto p = PlacementNew<Foo>(alloc);
        p->X = 42;
        PlacementDelete(alloc, p);
    }
    SECTION("LinearPageAllocator")
    {
        using pomdog::detail::LinearPageAllocator;
        LinearPageAllocator alloc;

        alloc.Reset(1024);

        auto p = PlacementNew<Foo>(alloc);
        p->X = 42;
        PlacementDelete(alloc, p);
    }
}
