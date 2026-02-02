// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/linear_page_allocator.h"
#include "pomdog/memory/placement_new.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::memory::placementDelete;
using pomdog::memory::placementNew;

TEST_CASE("PlacementNew")
{
    struct alignas(16) Foo final {
        std::int32_t X = 0;
        std::int32_t Y = 0;
        std::int32_t Z = 0;
        std::int32_t W = 0;
    };

    SUBCASE("LinearAllocator")
    {
        pomdog::memory::LinearAllocator alloc;

        std::array<std::uint8_t, 1024> buffer;
        alloc.reset(buffer);

        auto p = placementNew<Foo>(alloc);
        p->X = 42;
        placementDelete(alloc, p);
    }
    SUBCASE("LinearPageAllocator")
    {
        pomdog::memory::LinearPageAllocator alloc;
        alloc.reset(1024);

        auto p = placementNew<Foo>(alloc);
        p->X = 42;
        placementDelete(alloc, p);
    }
}
