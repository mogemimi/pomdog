// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT ConstantBuffer final {
public:
    ConstantBuffer() = delete;
    ConstantBuffer(const ConstantBuffer&) = delete;
    ConstantBuffer& operator=(const ConstantBuffer&) = delete;

    ConstantBuffer(
        std::unique_ptr<Buffer>&& nativeBuffer,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~ConstantBuffer();

    /// Sets constant buffer data.
    void setData(std::size_t offsetInBytes, std::span<const std::byte> source);

    /// Gets the size in bytes of this constant buffer.
    [[nodiscard]] std::size_t
    getSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this constant buffer.
    [[nodiscard]] BufferUsage
    getBufferUsage() const noexcept;

    /// Gets the pointer of the native constant buffer resource.
    [[nodiscard]] Buffer*
    getBuffer();

private:
    std::unique_ptr<Buffer> nativeBuffer_;
    std::uint32_t sizeInBytes_;
    BufferUsage bufferUsage_;
};

template <typename T>
[[nodiscard]] std::span<const std::byte> makeByteSpan(const T& data) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
    static_assert(!std::is_pointer_v<T>, "T is not pointer.");
    static_assert(!std::is_array_v<T>, "T is not array.");
    static_assert(!std::is_fundamental_v<T>);
    const std::span<const T, 1> s{&data, 1};
    return std::as_bytes(s);
}

} // namespace pomdog::gpu
