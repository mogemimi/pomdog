// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INDEXBUFFERGL4_752BED0C_HPP
#define POMDOG_INDEXBUFFERGL4_752BED0C_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using IndexBufferObjectGL4 = Tagged<GLuint, IndexBuffer>;

class IndexBufferGL4 final : public NativeBuffer {
public:
    IndexBufferGL4() = delete;

    IndexBufferGL4(std::size_t sizeInBytes, BufferUsage bufferUsage);

    IndexBufferGL4(void const* indices, std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~IndexBufferGL4() override;

    void GetData(std::size_t byteWidth, void* result) const override;

    void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t sizeInBytes) override;

    void BindBuffer();

private:
    Optional<IndexBufferObjectGL4> bufferObject;
};

} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_INDEXBUFFERGL4_752BED0C_HPP
