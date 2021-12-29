// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/native_buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

template <class Tag>
using BufferObjectGL4 = pomdog::detail::Tagged<GLuint, Tag>;

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

} // namespace pomdog::gpu::detail::gl4
