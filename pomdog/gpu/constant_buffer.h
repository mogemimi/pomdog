// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT ConstantBuffer final {
public:
    ConstantBuffer() = delete;
    ConstantBuffer(const ConstantBuffer&) = delete;
    ConstantBuffer& operator=(const ConstantBuffer&) = delete;

    ConstantBuffer(
        std::unique_ptr<detail::NativeBuffer>&& nativeBuffer,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~ConstantBuffer();

    /// Sets constant buffer data.
    template <typename T>
    void SetValue(const T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
        static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
        static_assert(!std::is_pointer<T>::value, "T is not pointer.");
        static_assert(!std::is_fundamental_v<T>);
        this->SetData(static_cast<const void*>(&value), sizeof(value));
    }

    /// Sets constant buffer data.
    template <typename T>
    void SetValue(const T* data, std::size_t count)
    {
        static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
        static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
        static_assert(!std::is_pointer<T>::value, "T is not pointer.");
        static_assert(!std::is_same_v<T, bool>);
        this->SetData(static_cast<const void*>(data), sizeof(T) * count);
    }

    /// Sets constant buffer data.
    void SetData(const void* source, std::size_t sizeInBytes);

    /// Sets constant buffer data.
    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes);

    /// Gets the size in bytes of this constant buffer.
    [[nodiscard]] std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this constant buffer.
    [[nodiscard]] BufferUsage GetBufferUsage() const noexcept;

    /// Gets the pointer of the native constant buffer resource.
    [[nodiscard]] detail::NativeBuffer* GetNativeBuffer();

private:
    std::unique_ptr<detail::NativeBuffer> nativeBuffer;
    std::uint32_t sizeInBytes;
    BufferUsage bufferUsage;
};

} // namespace pomdog::gpu
