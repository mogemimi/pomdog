//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DepthStencilState.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DepthStencilState::DepthStencilState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	DepthStencilDescription const& description)
	: nativeState(graphicsDevice->GetNativeGraphicsDevice()->CreateDepthStencilState(description))
	, description(description)
{}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState> DepthStencilState::CreateReadWriteDepth(
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = true;
	desc.DepthBufferWriteEnable = true;
	POMDOG_ASSERT(desc.StencilEnable == false);
	
	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState> DepthStencilState::CreateReadOnlyDepth(
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = true;
	desc.DepthBufferWriteEnable = false;
	POMDOG_ASSERT(desc.StencilEnable == false);
	
	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState> DepthStencilState::CreateNone(
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	DepthStencilDescription desc;
	desc.DepthBufferEnable = false;
	desc.DepthBufferWriteEnable = false;
	POMDOG_ASSERT(desc.StencilEnable == false);
	
	return std::make_shared<DepthStencilState>(graphicsDevice, desc);
}

}// namespace Pomdog
