// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

/// A simple bump/arena allocator that allocates memory linearly from a contiguous buffer.
///
/// Memory is allocated by advancing a pointer through a pre-assigned buffer.
/// Allocations are aligned to the requested alignment boundary.
///
/// Individual deallocations are no-ops; all allocated memory is released at once
/// by calling `reset()`. This makes the allocator very fast but unsuitable for
/// scenarios requiring individual object lifetime management.
///
/// The allocator does not own the underlying buffer. The caller is responsible
/// for ensuring the buffer outlives the allocator and all pointers obtained from it.
class LinearAllocator final {
private:
    std::span<std::uint8_t> buffer_;
    std::size_t allocatedSize_ = 0;

public:
    /// Assigns a new buffer and resets the allocation state.
    ///
    /// @param buffer The memory region to allocate from.
    void reset(std::span<std::uint8_t> buffer) noexcept;

    /// Resets the allocator, releasing the buffer reference and clearing all allocation state.
    void reset() noexcept;

    /// Returns the total number of bytes consumed from the buffer, including alignment padding.
    [[nodiscard]] std::size_t
    getAllocatedSize() const noexcept;

    /// Returns the total size of the underlying buffer in bytes.
    [[nodiscard]] std::size_t
    getBufferSize() const noexcept;

    /// Allocates a block of memory with the specified size and alignment.
    ///
    /// @param size The number of bytes to allocate. Must be > 0.
    /// @param alignment The alignment boundary in bytes. Must be a power of two.
    /// @return A pointer to the allocated memory aligned to `alignment`, or nullptr
    ///         if the allocation fails (e.g., out of memory, size is 0, or alignment is invalid).
    [[nodiscard]] unsafe_ptr<void>
    allocate(std::size_t size, std::size_t alignment) noexcept;

    /// No-op. Linear allocators do not support individual deallocation.
    ///
    /// To reclaim memory, call `reset()`.
    void deallocate(unsafe_ptr<const void> ptr) noexcept;

    /// Returns true if the given pointer falls within this allocator's buffer range.
    ///
    /// @param ptr The pointer to check. Must not be nullptr.
    [[nodiscard]] bool
    isOwnerOf(unsafe_ptr<const void> ptr) noexcept;
};

} // namespace pomdog::memory
