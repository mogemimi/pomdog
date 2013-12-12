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
	: nativeDepthStencilState(graphicsDevice->GetNativeGraphicsDevice()->CreateDepthStencilState(description))
	, description(description)
{
	POMDOG_ASSERT(nativeDepthStencilState);
}
//-----------------------------------------------------------------------
DepthStencilState::~DepthStencilState()
{
}
//-----------------------------------------------------------------------
DepthStencilOperation const& DepthStencilState::GetClockwiseFace() const
{
	return description.ClockwiseFace;
}
//-----------------------------------------------------------------------
DepthStencilOperation const& DepthStencilState::GetCounterClockwiseFace() const
{
	return description.CounterClockwiseFace;
}
//-----------------------------------------------------------------------
ComparisonFunction DepthStencilState::GetDepthBufferFunction() const
{
	return description.DepthBufferFunction;
}
//-----------------------------------------------------------------------
std::int32_t DepthStencilState::GetReferenceStencil() const
{
	return description.ReferenceStencil;
}
//-----------------------------------------------------------------------
std::uint32_t DepthStencilState::GetStencilMask() const
{
	return description.StencilMask;
}
//-----------------------------------------------------------------------
std::uint32_t DepthStencilState::GetStencilWriteMask() const
{
	return description.StencilWriteMask;
}
//-----------------------------------------------------------------------
bool DepthStencilState::IsDepthBufferEnable() const
{
	return description.DepthBufferEnable;
}
//-----------------------------------------------------------------------
bool DepthStencilState::IsDepthBufferWriteEnable() const
{
	return description.DepthBufferWriteEnable;
}
//-----------------------------------------------------------------------
bool DepthStencilState::IsStencilEnable() const
{
	return description.StencilEnable;
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
Details::RenderSystem::NativeDepthStencilState* DepthStencilState::GetNativeDepthStencilState()
{
	return nativeDepthStencilState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
