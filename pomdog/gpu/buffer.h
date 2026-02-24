// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

namespace pomdog::gpu {

class Buffer {
public:
    Buffer() noexcept;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    virtual ~Buffer();

    virtual void getData(
        u32 offsetInBytes,
        void* destination,
        u32 sizeInBytes) const = 0;

    virtual void setData(
        u32 offsetInBytes,
        const void* source,
        u32 sizeInBytes) = 0;
};

} // namespace pomdog::gpu
