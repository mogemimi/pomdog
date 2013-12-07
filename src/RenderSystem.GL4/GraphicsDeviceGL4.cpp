//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsDeviceGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include <Pomdog/Utility/Exception.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

//-----------------------------------------------------------------------
std::unique_ptr<NativeBlendState>
GraphicsDeviceGL4::CreateBlendState(BlendDescription const& description)
{
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "GraphicsDeviceGL4::CreateBlendState");
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeDepthStencilState>
GraphicsDeviceGL4::CreateDepthStencilState(DepthStencilDescription const& description)
{
	//return std::make_unique<DepthStencilStateGL4>(description); // C++14
	return std::unique_ptr<DepthStencilStateGL4>(new DepthStencilStateGL4(description));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceGL4::CreateSamplerState(SamplerDescription const& description)
{
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "GraphicsDeviceGL4::CreateSamplerState");
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRasterizerState>
GraphicsDeviceGL4::CreateRasterizerState(RasterizerDescription const& description)
{
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "GraphicsDeviceGL4::CreateRasterizerState");
}
//-----------------------------------------------------------------------
	
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
