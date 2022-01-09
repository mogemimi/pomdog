// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_allocator.h"
#include "pomdog/memory/alignment.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail {

void LinearAllocator::Reset(std::span<std::uint8_t> buffer) noexcept
{
    buffer_ = std::move(buffer);
    allocatedSize_ = 0;
}

void LinearAllocator::Reset() noexcept
{
    buffer_ = std::span<std::uint8_t>{};
    allocatedSize_ = 0;
}

[[nodiscard]] std::size_t LinearAllocator::GetAllocatedSize() const noexcept
{
    return allocatedSize_;
}

[[nodiscard]] std::size_t LinearAllocator::GetBufferSize() const noexcept
{
    return buffer_.size();
}

unsafe_ptr<void> LinearAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
    if (size == 0) {
        return nullptr;
    }
    if (!IsPowerOfTwo(alignment)) {
        return nullptr;
    }

    const auto oldAllocatedSize = allocatedSize_;

    const auto bufferHead = buffer_.data() + oldAllocatedSize;
    const auto allocStart = static_cast<std::uint8_t*>(AlignTo(bufferHead, alignment));
    POMDOG_ASSERT(bufferHead <= allocStart);

    const auto allocEnd = allocStart + size;
    const auto allocSize = allocEnd - bufferHead;
    POMDOG_ASSERT(allocStart < allocEnd);

    const auto newAllocatedSize = oldAllocatedSize + allocSize;
    if (newAllocatedSize > buffer_.size()) {
        // NOTE: out of memory.
        return nullptr;
    }

    POMDOG_ASSERT(static_cast<std::size_t>(allocStart - buffer_.data()) <= buffer_.size());
    POMDOG_ASSERT(static_cast<std::size_t>(allocEnd - buffer_.data()) <= buffer_.size());
    POMDOG_ASSERT(newAllocatedSize <= buffer_.size());

    allocatedSize_ = newAllocatedSize;

    return allocStart;
}

void LinearAllocator::Deallocate([[maybe_unused]] unsafe_ptr<const void> ptr) noexcept
{
    // NOTE: Do nothing
}

bool LinearAllocator::IsOwnerOf(unsafe_ptr<const void> ptr) noexcept
{
    POMDOG_ASSERT(ptr != nullptr);
    POMDOG_ASSERT(!buffer_.empty());

    const auto p = reinterpret_cast<uintptr_t>(ptr);
    const auto begin = reinterpret_cast<uintptr_t>(buffer_.data());
    const auto end = begin + buffer_.size();
    return (begin <= p) && (p < end);
}

} // namespace pomdog::detail
