// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/linear_page_allocator.h"
#include "pomdog/memory/placement_new.h"
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
        pomdog::memory::LinearAllocator alloc;

        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        auto p = pomdog::memory::placementNew<Foo>(alloc);
        p->X = 42;
        pomdog::memory::placementDelete(alloc, p);
    }
    SECTION("LinearPageAllocator")
    {
        pomdog::memory::LinearPageAllocator alloc;

        alloc.reset(1024);

        auto p = pomdog::memory::placementNew<Foo>(alloc);
        p->X = 42;
        pomdog::memory::placementDelete(alloc, p);
    }
}
