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

class LinearAllocator final {
private:
    std::span<std::uint8_t> buffer_;
    std::size_t allocatedSize_ = 0;

public:
    void reset(std::span<std::uint8_t> buffer) noexcept;

    void reset() noexcept;

    [[nodiscard]] std::size_t
    getAllocatedSize() const noexcept;

    [[nodiscard]] std::size_t
    getBufferSize() const noexcept;

    [[nodiscard]] unsafe_ptr<void>
    allocate(std::size_t size, std::size_t alignment) noexcept;

    void deallocate(unsafe_ptr<const void> ptr) noexcept;

    [[nodiscard]] bool
    isOwnerOf(unsafe_ptr<const void> ptr) noexcept;
};

} // namespace pomdog::memory
