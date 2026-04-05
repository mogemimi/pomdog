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

namespace pomdog::memory {

/// A paged linear allocator that automatically allocates new pages when the current page is exhausted.
///
/// This allocator manages a list of fixed-size pages, each backed by a LinearAllocator.
/// When an allocation cannot fit in the current page, it moves to the next page
/// (creating a new one if necessary).
///
/// Allocations that require more than pageSize bytes (including worst-case alignment padding)
/// will fail and return nullptr.
///
/// Individual deallocations are no-ops. Call `reset()` to reclaim all memory and reuse pages.
/// The alignment parameter must be a power of two.
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
    /// Sets the page size and resets all existing pages to the new size.
    ///
    /// All previously allocated memory becomes invalid after this call.
    ///
    /// @param pageSize The size of each page in bytes.
    void reset(std::size_t pageSize);

    /// Resets all pages without changing the page size, allowing pages to be reused.
    ///
    /// All previously allocated memory becomes invalid after this call.
    void reset() noexcept;

    /// Allocates a block of memory with the specified size and alignment.
    ///
    /// If the current page cannot satisfy the request, the allocator moves to the next page.
    /// A new page is created if no more pages are available.
    ///
    /// @param size The number of bytes to allocate. Must be > 0 and <= pageSize.
    /// @param alignment The alignment boundary in bytes. Must be a power of two.
    /// @return A pointer to the allocated memory aligned to `alignment`, or nullptr
    ///         if the allocation fails (e.g., size exceeds page capacity including alignment padding).
    [[nodiscard]] unsafe_ptr<void>
    allocate(std::size_t size, std::size_t alignment);

    /// No-op. Linear page allocators do not support individual deallocation.
    ///
    /// To reclaim memory, call `reset()`.
    void deallocate(unsafe_ptr<const void> ptr) noexcept;
};

} // namespace pomdog::memory
