// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/linear_page_allocator.h"
#include "pomdog/memory/alignment.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail {

void LinearPageAllocator::Reset(std::size_t pageSize) noexcept
{
    pageSize_ = pageSize;
    for (auto& page : pages_) {
        page.buffer.resize(pageSize_);
        page.buffer.shrink_to_fit();
        page.allocator.Reset(page.buffer);
    }
    pageIndex_ = 0;
}

void LinearPageAllocator::Reset() noexcept
{
    for (auto& page : pages_) {
        page.allocator.Reset(page.buffer);
    }
    pageIndex_ = 0;
}

unsafe_ptr<void> LinearPageAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
    if (size > pageSize_) {
        return nullptr;
    }

    if (pages_.empty()) {
        Page page;
        page.buffer.resize(pageSize_);
        page.allocator.Reset(page.buffer);
        pages_.push_back(std::move(page));
    }

    {
        auto& page = pages_[pageIndex_];
        if (auto ptr = page.allocator.Allocate(size, alignment); ptr != nullptr) {
            return ptr;
        }
    }

    pageIndex_ += 1;

    if (pageIndex_ >= pages_.size()) {
        Page page;
        page.buffer.resize(pageSize_);
        page.allocator.Reset(page.buffer);
        pages_.push_back(std::move(page));
    }

    auto& page = pages_[pageIndex_];
    return page.allocator.Allocate(size, alignment);
}

void LinearPageAllocator::Deallocate([[maybe_unused]] unsafe_ptr<const void> ptr) noexcept
{
}

} // namespace pomdog::detail
