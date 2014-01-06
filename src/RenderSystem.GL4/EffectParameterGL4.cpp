//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectParameterGL4.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include "../Utility/MakeUnique.hpp"
#include "ConstantBufferGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
EffectParameterGL4::EffectParameterGL4(std::uint32_t byteWidth)
{
	constantBuffer = MakeUnique<ConstantBufferGL4>(byteWidth);
}
//-----------------------------------------------------------------------
EffectParameterGL4::~EffectParameterGL4()
{
}
//-----------------------------------------------------------------------
void EffectParameterGL4::Apply(std::uint32_t slotIndex)
{
	POMDOG_ASSERT(constantBuffer);
	constantBuffer->Apply(slotIndex);
}
//-----------------------------------------------------------------------
void EffectParameterGL4::GetValue(std::uint32_t byteWidth, std::uint8_t* result) const
{
	POMDOG_ASSERT(byteWidth > 0);
	constantBuffer->GetData(byteWidth, result);
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(std::uint8_t const* data, std::uint32_t byteWidth)
{
	POMDOG_ASSERT(byteWidth > 0);
	constantBuffer->SetData(data, byteWidth);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
