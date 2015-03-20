// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/DepthStencilState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DepthStencilState::DepthStencilState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	DepthStencilDescription const& descriptionIn)
	: nativeDepthStencilState(graphicsDevice->NativeGraphicsDevice()->CreateDepthStencilState(descriptionIn))
	, description(descriptionIn)
{
	POMDOG_ASSERT(nativeDepthStencilState);
}
//-----------------------------------------------------------------------
DepthStencilState::~DepthStencilState() = default;
//-----------------------------------------------------------------------
DepthStencilDescription DepthStencilState::Description() const
{
	return description;
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState>
DepthStencilState::CreateReadWriteDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = true;
	desc.DepthBufferWriteEnable = true;
	POMDOG_ASSERT(desc.StencilEnable == false);

	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState>
DepthStencilState::CreateReadOnlyDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = true;
	desc.DepthBufferWriteEnable = false;
	POMDOG_ASSERT(desc.StencilEnable == false);

	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState>
DepthStencilState::CreateNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = false;
	desc.DepthBufferWriteEnable = false;
	POMDOG_ASSERT(desc.StencilEnable == false);

	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeDepthStencilState* DepthStencilState::NativeDepthStencilState()
{
	return nativeDepthStencilState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
