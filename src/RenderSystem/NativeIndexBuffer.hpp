// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEINDEXBUFFER_C6FFE20E_HPP
#define POMDOG_NATIVEINDEXBUFFER_C6FFE20E_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeIndexBuffer {
public:
    NativeIndexBuffer() = default;
    NativeIndexBuffer(NativeIndexBuffer const&) = delete;
    NativeIndexBuffer & operator=(NativeIndexBuffer const&) = delete;

    virtual ~NativeIndexBuffer() = default;

    virtual void SetData(std::uint32_t offsetInBytes,
        void const* source, std::uint32_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_NATIVEINDEXBUFFER_C6FFE20E_HPP
