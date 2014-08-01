//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
GraphicsDevice::GraphicsDevice(std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> && device)
	: nativeGraphicsDevice(std::move(device))
{}
//-----------------------------------------------------------------------
GraphicsDevice::~GraphicsDevice() = default;
//-----------------------------------------------------------------------
Details::RenderSystem::NativeGraphicsDevice* GraphicsDevice::NativeGraphicsDevice()
{
	POMDOG_ASSERT(nativeGraphicsDevice);
	return nativeGraphicsDevice.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
