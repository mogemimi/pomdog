// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
enum class BufferUsage : u8;
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

template <class Tag>
using BufferObjectGL4 = pomdog::detail::Tagged<GLuint, Tag>;

template <class Tag>
class BufferGL4 final : public Buffer {
private:
    using BufferObject = BufferObjectGL4<Tag>;
    std::optional<BufferObject> bufferObject_;
    MemoryUsage memoryUsage_;

public:
    ~BufferGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const void* sourceData,
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const BufferDesc& desc,
        std::span<const u8> sourceData) noexcept;

    void getData(
        u32 offsetInBytes,
        void* destination,
        u32 sizeInBytes) const override;

    void setData(
        u32 offsetInBytes,
        const void* source,
        u32 sizeInBytes) override;

    [[nodiscard]] std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) override;

    void unmap() override;

    void bindBuffer();

    /// Gets the pointer of the native buffer.
    [[nodiscard]] GLuint
    getBuffer() const noexcept;
};

using ConstantBufferGL4 = BufferGL4<ConstantBuffer>;
using IndexBufferGL4 = BufferGL4<IndexBuffer>;
using VertexBufferGL4 = BufferGL4<VertexBuffer>;

} // namespace pomdog::gpu::detail::gl4
