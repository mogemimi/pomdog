// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP
#define POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeVertexBuffer {
public:
    NativeVertexBuffer() = default;
    NativeVertexBuffer(NativeVertexBuffer const&) = delete;
    NativeVertexBuffer & operator=(NativeVertexBuffer const&) = delete;

    virtual ~NativeVertexBuffer() = default;

    virtual void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP)
