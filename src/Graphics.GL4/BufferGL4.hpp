// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Graphics.Backends/NativeBuffer.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <optional>

namespace Pomdog::Detail::GL4 {

template <class Tag>
using BufferObjectGL4 = Tagged<GLuint, Tag>;

template <class Tag>
class BufferGL4 final : public NativeBuffer {
public:
    ~BufferGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept;

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
    using BufferObject = BufferObjectGL4<Tag>;
    std::optional<BufferObjectGL4<Tag>> bufferObject;
};

using ConstantBufferGL4 = BufferGL4<ConstantBuffer>;
using IndexBufferGL4 = BufferGL4<IndexBuffer>;
using VertexBufferGL4 = BufferGL4<VertexBuffer>;

} // namespace Pomdog::Detail::GL4
