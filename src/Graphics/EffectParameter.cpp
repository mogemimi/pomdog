//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectParameter.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeConstantBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../Utility/MakeUnique.hpp"

namespace Pomdog {
namespace {

class DummyConstantBuffer final: public Details::RenderSystem::NativeConstantBuffer
{
public:
	void GetData(std::uint32_t, std::uint8_t*) const override {}
	
	void SetData(std::uint8_t const*, std::uint32_t) override {}
};

}// unnamed namespace
//-----------------------------------------------------------------------
EffectParameter::EffectParameter()
	: nativeConstantBuffer(Details::MakeUnique<DummyConstantBuffer>())
{
}
//-----------------------------------------------------------------------
EffectParameter::EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t byteConstants)
	: nativeConstantBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateConstantBuffer(byteConstants))
{
	POMDOG_ASSERT(byteConstants > 0);
}
//-----------------------------------------------------------------------
EffectParameter::~EffectParameter()
{
}
//-----------------------------------------------------------------------
void EffectParameter::GetValue(std::uint32_t byteWidth, std::uint8_t * result) const
{
	POMDOG_ASSERT(byteWidth > 0);
	POMDOG_ASSERT(result != nullptr);
	POMDOG_ASSERT(nativeConstantBuffer);
	nativeConstantBuffer->GetData(byteWidth, result);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(std::uint8_t const* data, std::uint32_t byteWidth)
{
	POMDOG_ASSERT(data != nullptr);
	POMDOG_ASSERT(byteWidth > 0);
	POMDOG_ASSERT(nativeConstantBuffer);
	return nativeConstantBuffer->SetData(data, byteWidth);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeConstantBuffer* EffectParameter::GetNativeConstantBuffer()
{
	return nativeConstantBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
