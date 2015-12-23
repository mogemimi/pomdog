// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/detail/EffectBinaryParameter.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>

namespace Pomdog {

class POMDOG_EXPORT ConstantBuffer {
public:
    ConstantBuffer() = delete;
    ConstantBuffer(ConstantBuffer const&) = delete;
    ConstantBuffer & operator=(ConstantBuffer const&) = delete;

    ConstantBuffer(GraphicsDevice & graphicsDevice,
        std::size_t sizeInBytes,
        Pomdog::BufferUsage bufferUsage);

    ConstantBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::size_t sizeInBytes,
        Pomdog::BufferUsage bufferUsage);

    ConstantBuffer(GraphicsDevice & graphicsDevice,
        void const* sourceData,
        std::size_t sizeInBytes,
        Pomdog::BufferUsage bufferUsage);

    ConstantBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        void const* sourceData,
        std::size_t sizeInBytes,
        Pomdog::BufferUsage bufferUsage);

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

    template <typename T>
    void SetValue(T const& value)
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        Detail::EffectBinaryParameter::Set(*this, value);
    }

    template <typename T>
    void SetValue(T const* data, std::size_t count)
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        Detail::EffectBinaryParameter::Set(*this, data, count);
    }

    void SetValue(void const* data, std::size_t sizeInBytes);

    std::size_t SizeInBytes() const noexcept;

    Pomdog::BufferUsage BufferUsage() const noexcept;

public:
    Detail::NativeBuffer* NativeConstantBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeConstantBuffer;
    std::uint32_t sizeInBytes;
    Pomdog::BufferUsage bufferUsage;
};

} // namespace Pomdog
