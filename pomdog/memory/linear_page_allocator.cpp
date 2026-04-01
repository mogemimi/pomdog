// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_page_allocator.h"
#include "pomdog/memory/alignment.h"
#include "pomdog/utility/assert.h"

namespace pomdog::memory {

void LinearPageAllocator::reset(std::size_t pageSize)
{
    pageSize_ = pageSize;
    for (auto& page : pages_) {
        page.buffer.resize(pageSize_);
        page.buffer.shrink_to_fit();
        page.allocator.reset(page.buffer);
    }
    pageIndex_ = 0;
}

void LinearPageAllocator::reset() noexcept
{
    for (auto& page : pages_) {
        page.allocator.reset(page.buffer);
    }
    pageIndex_ = 0;
}

unsafe_ptr<void> LinearPageAllocator::allocate(std::size_t size, std::size_t alignment)
{
    if (size == 0) {
        return nullptr;
    }
    if (!isPowerOfTwo(alignment)) {
        return nullptr;
    }

    // NOTE: Reject allocations that cannot fit in a fresh page even in the worst case.
    // The worst-case alignment padding is (alignment - 1) bytes.
    if (size + (alignment - 1) > pageSize_) {
        return nullptr;
    }

    if (pages_.empty()) {
        Page page;
        page.buffer.resize(pageSize_);
        page.allocator.reset(page.buffer);
        pages_.push_back(std::move(page));
    }

    {
        auto& page = pages_[pageIndex_];
        if (auto ptr = page.allocator.allocate(size, alignment); ptr != nullptr) {
            return ptr;
        }
        if (page.allocator.getAllocatedSize() == 0) {
            // NOTE: If allocation failed but no memory was allocated, it means the requested size is too large for this page.
            // In this case, we should not try to allocate from the next page, because it will also fail.
            return nullptr;
        }
    }

    pageIndex_ += 1;

    if (pageIndex_ >= pages_.size()) {
        Page page;
        page.buffer.resize(pageSize_);
        page.allocator.reset(page.buffer);
        pages_.push_back(std::move(page));
    }

    auto& page = pages_[pageIndex_];
    return page.allocator.allocate(size, alignment);
}

void LinearPageAllocator::deallocate([[maybe_unused]] unsafe_ptr<const void> ptr) noexcept
{
}

} // namespace pomdog::memory
