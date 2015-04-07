// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTBUFFER_6D116595_HPP
#define POMDOG_CONSTANTBUFFER_6D116595_HPP

#include "detail/ForwardDeclarations.hpp"
#include "detail/EffectBinaryParameter.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>

namespace Pomdog {

class POMDOG_EXPORT ConstantBuffer {
public:
    ConstantBuffer() = default;

    ConstantBuffer(ConstantBuffer const&) = delete;
    ConstantBuffer & operator=(ConstantBuffer const&) = delete;

    ConstantBuffer(GraphicsDevice & graphicsDevice,
        std::size_t byteConstants);

    ConstantBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::size_t byteConstants);

    ~ConstantBuffer();

    template <typename T>
    T GetValue() const
    {
        static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
        T result;
        Detail::EffectBinaryParameter::Get<T>(*this, result);
        return std::move(result);
    }

    void GetValue(std::size_t byteWidth, void* result) const;

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

    void SetValue(void const* data, std::size_t byteWidth);

public:
    Detail::RenderSystem::NativeBuffer* NativeConstantBuffer();

private:
    std::unique_ptr<Detail::RenderSystem::NativeBuffer> nativeConstantBuffer;
};

} // namespace Pomdog

#endif // POMDOG_CONSTANTBUFFER_6D116595_HPP
