//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/BlendState.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeBlendState.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
BlendState::BlendState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	BlendDescription const& description)
	: nativeBlendState(graphicsDevice->GetNativeGraphicsDevice()->CreateBlendState(description))
	, description(description)
{
	POMDOG_ASSERT(nativeBlendState);
}
//-----------------------------------------------------------------------
BlendState::~BlendState()
{
}
//-----------------------------------------------------------------------
Color BlendState::BlendFactor() const
{
	return description.BlendFactor;
}
//-----------------------------------------------------------------------
BlendFunction BlendState::GetColorBlendFunction() const
{
	return description.ColorBlendFunction;
}
//-----------------------------------------------------------------------
BlendFunction BlendState::GetAlphaBlendFunction() const
{
	return description.AlphaBlendFunction;
}
//-----------------------------------------------------------------------
Blend BlendState::GetColorSourceBlend() const
{
	return description.ColorSourceBlend;
}
//-----------------------------------------------------------------------
Blend BlendState::GetAlphaSourceBlend() const
{
	return description.AlphaSourceBlend;
}
//-----------------------------------------------------------------------
Blend BlendState::GetColorDestinationBlend() const
{
	return description.ColorDestinationBlend;
}
//-----------------------------------------------------------------------
Blend BlendState::GetAlphaDestinationBlend() const
{
	return description.AlphaDestinationBlend;
}
//-----------------------------------------------------------------------
std::uint32_t BlendState::GetMultiSampleMask() const
{
	return description.MultiSampleMask;
}
//-----------------------------------------------------------------------
bool BlendState::IsAlphaToCoverageEnable() const
{
	return description.AlphaToCoverageEnable;
}
//-----------------------------------------------------------------------
std::shared_ptr<BlendState>
BlendState::CreateAdditive(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	BlendDescription desc;
	desc.ColorBlendFunction = BlendFunction::Add;
	desc.AlphaBlendFunction = BlendFunction::Add;
	desc.ColorSourceBlend = Blend::SourceAlpha;
	desc.AlphaSourceBlend = Blend::SourceAlpha;
	desc.ColorDestinationBlend = Blend::One;
	desc.AlphaDestinationBlend = Blend::One;
	POMDOG_ASSERT(desc.BlendFactor == Color::White);

	return std::make_shared<BlendState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<BlendState>
BlendState::CreateAlphaBlend(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	BlendDescription desc;
	desc.ColorBlendFunction = BlendFunction::Add;
	desc.AlphaBlendFunction = BlendFunction::Add;
	desc.ColorSourceBlend = Blend::One;
	desc.AlphaSourceBlend = Blend::One;
	desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
	desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
	POMDOG_ASSERT(desc.BlendFactor == Color::White);

	return std::make_shared<BlendState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<BlendState>
BlendState::CreateNonPremultiplied(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	BlendDescription desc;
	desc.ColorBlendFunction = BlendFunction::Add;
	desc.AlphaBlendFunction = BlendFunction::Add;
	desc.ColorSourceBlend = Blend::SourceAlpha;
	desc.AlphaSourceBlend = Blend::SourceAlpha;
	desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
	desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
	POMDOG_ASSERT(desc.BlendFactor == Color::White);

	return std::make_shared<BlendState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<BlendState>
BlendState::CreateOpaque(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	BlendDescription desc;
	desc.ColorBlendFunction = BlendFunction::Add;
	desc.AlphaBlendFunction = BlendFunction::Add;
	desc.ColorSourceBlend = Blend::One;
	desc.AlphaSourceBlend = Blend::One;
	desc.ColorDestinationBlend = Blend::Zero;
	desc.AlphaDestinationBlend = Blend::Zero;
	POMDOG_ASSERT(desc.BlendFactor == Color::White);

	return std::make_shared<BlendState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeBlendState* BlendState::GetNativeBlendState()
{
	return nativeBlendState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
