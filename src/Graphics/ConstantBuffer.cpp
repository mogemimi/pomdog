// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(GraphicsDevice & graphicsDevice,
    std::size_t byteConstants)
    : nativeConstantBuffer(graphicsDevice.NativeGraphicsDevice()
        ->CreateConstantBuffer(byteConstants))
{
    POMDOG_ASSERT(byteConstants > 0);
}
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::size_t byteConstants)
    : ConstantBuffer(*graphicsDevice, byteConstants)
{}
//-----------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer() = default;
//-----------------------------------------------------------------------
void ConstantBuffer::GetValue(std::size_t byteWidth, void* result) const
{
    POMDOG_ASSERT(byteWidth > 0);
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(nativeConstantBuffer);
    nativeConstantBuffer->GetData(byteWidth, result);
}
//-----------------------------------------------------------------------
void ConstantBuffer::SetValue(void const* data, std::size_t byteWidth)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(byteWidth > 0);
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer->SetData(0, data, byteWidth);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeBuffer* ConstantBuffer::NativeConstantBuffer()
{
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
