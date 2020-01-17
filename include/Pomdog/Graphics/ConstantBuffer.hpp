// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/detail/EffectBinaryParameter.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT ConstantBuffer final {
public:
    ConstantBuffer() = delete;
    ConstantBuffer(const ConstantBuffer&) = delete;
    ConstantBuffer& operator=(const ConstantBuffer&) = delete;

    ConstantBuffer(
        GraphicsDevice& graphicsDevice,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ConstantBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ConstantBuffer(
        GraphicsDevice& graphicsDevice,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ConstantBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~ConstantBuffer();

    template <typename T>
    T GetValue() const
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        T result;
        Detail::EffectBinaryParameter::Get<T>(*this, result);
        return std::move(result);
    }

    void GetValue(std::size_t sizeInBytes, void* result) const;

    /// Sets constant buffer data.
    template <typename T>
    void SetValue(const T& value)
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        Detail::EffectBinaryParameter::Set(*this, value);
    }

    /// Sets constant buffer data.
    template <typename T>
    void SetValue(const T* data, std::size_t count)
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        Detail::EffectBinaryParameter::Set(*this, data, count);
    }

    /// Sets constant buffer data.
    void SetValue(const void* data, std::size_t sizeInBytes);

    /// Gets the size in bytes of this constant buffer.
    std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this constant buffer.
    BufferUsage GetBufferUsage() const noexcept;

    /// Gets the pointer of the native constant buffer resource.
    Detail::NativeBuffer* GetNativeConstantBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeConstantBuffer;
    std::uint32_t sizeInBytes;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
