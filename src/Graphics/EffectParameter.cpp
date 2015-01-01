//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/EffectParameter.hpp>
#include "../RenderSystem/NativeConstantBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
EffectParameter::EffectParameter(GraphicsDevice & graphicsDevice, std::uint32_t byteConstants)
	: nativeConstantBuffer(graphicsDevice.NativeGraphicsDevice()->CreateConstantBuffer(byteConstants))
{
	POMDOG_ASSERT(byteConstants > 0);
}
//-----------------------------------------------------------------------
EffectParameter::EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t byteConstants)
	: EffectParameter(*graphicsDevice, byteConstants)
{}
//-----------------------------------------------------------------------
EffectParameter::~EffectParameter() = default;
//-----------------------------------------------------------------------
void EffectParameter::GetValue(std::uint32_t byteWidth, void* result) const
{
	POMDOG_ASSERT(byteWidth > 0);
	POMDOG_ASSERT(result != nullptr);
	POMDOG_ASSERT(nativeConstantBuffer);
	nativeConstantBuffer->GetData(byteWidth, result);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(void const* data, std::uint32_t byteWidth)
{
	POMDOG_ASSERT(data != nullptr);
	POMDOG_ASSERT(byteWidth > 0);
	POMDOG_ASSERT(nativeConstantBuffer);
	return nativeConstantBuffer->SetData(0, data, byteWidth);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeConstantBuffer* EffectParameter::NativeConstantBuffer()
{
	POMDOG_ASSERT(nativeConstantBuffer);
	return nativeConstantBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
