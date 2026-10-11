// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/view_buffer.h"
#include "pomdog/memory/aligned_alloc.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs::detail {

void ViewBuffer::AlignedFree::operator()(u8* ptr) const noexcept
{
    memory::aligned_free(ptr);
}

std::unique_ptr<Error>
ViewBuffer::allocate(std::size_t size) noexcept
{
    reset();

    // NOTE: Games almost never load empty files, so the buffer accepts an
    // allocation of one alignment block for that exceptional case, and keeps
    // the implementation aimed at the files with content that make up nearly
    // all loads: data_ alone tells whether the buffer holds content, without a
    // separate flag for empty content.
    // NOTE: aligned_alloc() takes a size that is a multiple of the alignment.
    constexpr auto alignment = FileViewAlignment;
    if (size > std::numeric_limits<std::size_t>::max() - (alignment - 1)) {
        return errors::make(pomdog::format("cannot allocate {} bytes for a file view", size));
    }
    const auto allocationSize = std::max((size + (alignment - 1)) / alignment * alignment, alignment);

    auto* ptr = static_cast<u8*>(memory::aligned_alloc(alignment, allocationSize));
    if (ptr == nullptr) {
        return errors::make(pomdog::format("cannot allocate {} bytes for a file view", size));
    }
    data_.reset(ptr);
    size_ = size;
    return nullptr;
}

void ViewBuffer::reset() noexcept
{
    data_.reset();
    size_ = 0;
}

bool ViewBuffer::isAllocated() const noexcept
{
    return data_ != nullptr;
}

std::span<u8>
ViewBuffer::bytes() noexcept
{
    if (data_ == nullptr) {
        return {};
    }
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<u8>{data_.get(), size_};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

std::span<const u8>
ViewBuffer::bytes() const noexcept
{
    if (data_ == nullptr) {
        return {};
    }
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{data_.get(), size_};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

} // namespace pomdog::vfs::detail
