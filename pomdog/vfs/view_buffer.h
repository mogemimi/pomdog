// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs::detail {

/// ViewBuffer owns bytes that start at an address aligned to FileViewAlignment.
/// A file keeps its content in one to return it from File::view().
class ViewBuffer final {
private:
    /// AlignedFree frees the bytes that ViewBuffer allocates.
    struct AlignedFree final {
        void operator()(u8* ptr) const noexcept;
    };

    std::unique_ptr<u8, AlignedFree> data_;
    std::size_t size_ = 0;

public:
    ViewBuffer() noexcept = default;

    ViewBuffer(const ViewBuffer&) = delete;
    ViewBuffer& operator=(const ViewBuffer&) = delete;
    ViewBuffer(ViewBuffer&&) = delete;
    ViewBuffer& operator=(ViewBuffer&&) = delete;

    ~ViewBuffer() noexcept = default;

    /// Allocates `size` uninitialized bytes in place of the bytes it holds.
    /// It allocates memory even when `size` is 0, so that isAllocated() tells
    /// empty content apart from no content.
    [[nodiscard]] std::unique_ptr<Error>
    allocate(std::size_t size) noexcept;

    /// Releases the bytes.
    void reset() noexcept;

    /// Returns true after allocate() succeeds, until reset() releases the bytes.
    [[nodiscard]] bool
    isAllocated() const noexcept;

    /// Returns the allocated bytes, which are empty before allocate().
    [[nodiscard]] std::span<u8>
    bytes() noexcept;

    /// Returns the allocated bytes, which are empty before allocate().
    [[nodiscard]] std::span<const u8>
    bytes() const noexcept;
};

} // namespace pomdog::vfs::detail
