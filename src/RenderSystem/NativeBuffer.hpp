// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEBUFFER_B658AF8C_HPP
#define POMDOG_NATIVEBUFFER_B658AF8C_HPP

#include <cstddef>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeBuffer {
public:
    NativeBuffer() = default;
    NativeBuffer(NativeBuffer const&) = delete;
    NativeBuffer & operator=(NativeBuffer const&) = delete;

    virtual ~NativeBuffer() = default;

    virtual void GetData(std::size_t offsetInBytes,
        void* destination, std::size_t sizeInBytes) const = 0;

    virtual void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t sizeInBytes) = 0;
};

} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NATIVEBUFFER_B658AF8C_HPP
