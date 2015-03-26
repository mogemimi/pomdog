// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP
#define POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using VertexBufferObjectGL4 = Tagged<GLuint, VertexBuffer>;

class VertexBufferGL4 final: public NativeVertexBuffer {
public:
    VertexBufferGL4() = delete;
    ~VertexBufferGL4() override;

    VertexBufferGL4(std::size_t sizeInBytes, BufferUsage bufferUsage);

    VertexBufferGL4(void const* vertices, std::size_t sizeInBytes, BufferUsage bufferUsage);

    ///@copydoc NativeVertexBuffer
    void SetData(std::size_t offsetInBytes, void const* source,
        std::size_t sizeInBytes) override;

    void BindBuffer();

private:
    Optional<VertexBufferObjectGL4> bufferObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP)
