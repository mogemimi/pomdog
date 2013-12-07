//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DepthStencilState.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DepthStencilState::DepthStencilState(std::shared_ptr<GraphicsDevice> const& device,
	DepthStencilDescription const& description)
	: nativeState(device->GetNativeGraphicsDevice()->CreateDepthStencilState(description))
	, description(description)
{}
//-----------------------------------------------------------------------
}// namespace Pomdog
