// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class Buffer {
public:
    Buffer() noexcept;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    virtual ~Buffer();

    virtual void getData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const = 0;

    virtual void setData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) = 0;
};

} // namespace pomdog::gpu
