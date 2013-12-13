//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/RasterizerState.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeRasterizerState.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
RasterizerState::RasterizerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	RasterizerDescription const& description)
	: nativeRasterizerState(graphicsDevice->GetNativeGraphicsDevice()->CreateRasterizerState(description))
	, description(description)
{
	POMDOG_ASSERT(nativeRasterizerState);
}
//-----------------------------------------------------------------------
RasterizerState::~RasterizerState()
{
}
//-----------------------------------------------------------------------
std::shared_ptr<RasterizerState>
RasterizerState::CreateCullClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	RasterizerDescription desc;
	desc.CullMode = CullMode::ClockwiseFace;
	desc.FillMode = FillMode::Solid;

	return std::make_shared<RasterizerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<RasterizerState>
RasterizerState::CreateCullCounterClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	RasterizerDescription desc;
	desc.CullMode = CullMode::CounterClockwiseFace;
	desc.FillMode = FillMode::Solid;

	return std::make_shared<RasterizerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
std::shared_ptr<RasterizerState>
RasterizerState::CreateCullNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	RasterizerDescription desc;
	desc.CullMode = CullMode::None;
	desc.FillMode = FillMode::Solid;

	return std::make_shared<RasterizerState>(graphicsDevice, desc);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeRasterizerState* RasterizerState::GetNativeRasterizerState()
{
	return nativeRasterizerState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
