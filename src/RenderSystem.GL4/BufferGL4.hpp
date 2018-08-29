// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <optional>

namespace Pomdog {
namespace Detail {
namespace GL4 {

template <class Tag>
using BufferObjectGL4 = Tagged<GLuint, Tag>;

template <class Tag>
class BufferGL4 final : public NativeBuffer {
public:
    BufferGL4(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    BufferGL4(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~BufferGL4();

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    void BindBuffer();

    GLuint GetBuffer() const;

private:
    typedef BufferObjectGL4<Tag> BufferObject;
    std::optional<BufferObjectGL4<Tag>> bufferObject;
};

using ConstantBufferGL4 = BufferGL4<ConstantBuffer>;
using IndexBufferGL4 = BufferGL4<IndexBuffer>;
using VertexBufferGL4 = BufferGL4<VertexBuffer>;

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
