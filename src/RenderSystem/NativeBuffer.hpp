// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog::Detail {

class NativeBuffer {
public:
    NativeBuffer() = default;
    NativeBuffer(const NativeBuffer&) = delete;
    NativeBuffer & operator=(const NativeBuffer&) = delete;

    virtual ~NativeBuffer() = default;

    virtual void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const = 0;

    virtual void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) = 0;
};

} // namespace Pomdog::Detail
