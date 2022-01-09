// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class LinearPageAllocator final {
private:
    struct Page final {
        std::vector<std::uint8_t> buffer;
        LinearAllocator allocator;
    };

    std::vector<Page> pages_;
    std::size_t pageIndex_ = 0;
    std::size_t pageSize_ = 4096;

public:
    void Reset(std::size_t pageSize) noexcept;

    void Reset() noexcept;

    [[nodiscard]] unsafe_ptr<void> Allocate(std::size_t size, std::size_t alignment) noexcept;

    void Deallocate(unsafe_ptr<const void> ptr) noexcept;
};

} // namespace pomdog::detail
