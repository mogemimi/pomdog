// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

class BufferGL4 final : public Buffer {
private:
    std::optional<GLuint> bufferObject_;
    GLenum bufferTarget_ = 0;
    GLenum bufferBinding_ = 0;
    MemoryUsage memoryUsage_;

public:
    ~BufferGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const BufferDesc& desc,
        std::span<const u8> sourceData) noexcept;

    void getData(
        u32 offsetInBytes,
        std::span<u8> destination) const override;

    void setData(
        u32 offsetInBytes,
        std::span<const u8> source) override;

    [[nodiscard]] std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) override;

    void unmap() override;

    void bindBuffer();

    /// Gets the pointer of the native buffer.
    [[nodiscard]] GLuint
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::gl4
