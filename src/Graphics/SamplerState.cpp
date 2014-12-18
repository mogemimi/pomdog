//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/SamplerState.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
SamplerState::SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	SamplerDescription const& descriptionIn)
	: nativeSamplerState(graphicsDevice->NativeGraphicsDevice()->CreateSamplerState(descriptionIn))
	, description(descriptionIn)
{
	POMDOG_ASSERT(nativeSamplerState);
}
//-----------------------------------------------------------------------
SamplerState::~SamplerState() = default;
//-----------------------------------------------------------------------
SamplerDescription SamplerState::Description() const
{
	return description;
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreateAnisotropicClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Anisotropic;
	desc.AddressU = TextureAddressMode::Clamp;
	desc.AddressV = TextureAddressMode::Clamp;
	desc.AddressW = TextureAddressMode::Clamp;
	desc.MaxAnisotropy = 0;
	desc.MaxMipLevel = 1000U;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreateAnisotropicWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Anisotropic;
	desc.AddressU = TextureAddressMode::Wrap;
	desc.AddressV = TextureAddressMode::Wrap;
	desc.AddressW = TextureAddressMode::Wrap;
	desc.MaxAnisotropy = 0;
	desc.MaxMipLevel = 1000U;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreateLinearClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Linear;
	desc.AddressU = TextureAddressMode::Clamp;
	desc.AddressV = TextureAddressMode::Clamp;
	desc.AddressW = TextureAddressMode::Clamp;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreateLinearWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Linear;
	desc.AddressU = TextureAddressMode::Wrap;
	desc.AddressV = TextureAddressMode::Wrap;
	desc.AddressW = TextureAddressMode::Wrap;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreatePointClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Point;
	desc.AddressU = TextureAddressMode::Clamp;
	desc.AddressV = TextureAddressMode::Clamp;
	desc.AddressW = TextureAddressMode::Clamp;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<SamplerState>
SamplerState::CreatePointWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	SamplerDescription desc;
	desc.Filter = TextureFilter::Point;
	desc.AddressU = TextureAddressMode::Wrap;
	desc.AddressV = TextureAddressMode::Wrap;
	desc.AddressW = TextureAddressMode::Wrap;

	return std::make_shared<SamplerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeSamplerState* SamplerState::NativeSamplerState()
{
	return nativeSamplerState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
