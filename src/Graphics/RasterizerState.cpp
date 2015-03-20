//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/RasterizerState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeRasterizerState.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
RasterizerState::RasterizerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	RasterizerDescription const& descriptionIn)
	: nativeRasterizerState(graphicsDevice->NativeGraphicsDevice()->CreateRasterizerState(descriptionIn))
	, description(descriptionIn)
{
	POMDOG_ASSERT(nativeRasterizerState);
}
//-----------------------------------------------------------------------
RasterizerState::~RasterizerState() = default;
//-----------------------------------------------------------------------
RasterizerDescription RasterizerState::Description() const
{
	return description;
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
Detail::RenderSystem::NativeRasterizerState* RasterizerState::NativeRasterizerState()
{
	return nativeRasterizerState.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
