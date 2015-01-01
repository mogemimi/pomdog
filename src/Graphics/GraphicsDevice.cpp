//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <Pomdog/Graphics/detail/BuiltinShaderPool.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
class GraphicsDevice::Impl {
public:
	explicit Impl(std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> && device);

public:
	Details::BuiltinShaderPool shaderPool;
	std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> nativeGraphicsDevice;
};
//-----------------------------------------------------------------------
GraphicsDevice::Impl::Impl(std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> && device)
	: nativeGraphicsDevice(std::move(device))
{}
//-----------------------------------------------------------------------
GraphicsDevice::GraphicsDevice(std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> && device)
	: impl(std::make_unique<Impl>(std::move(device)))
{}
//-----------------------------------------------------------------------
GraphicsDevice::~GraphicsDevice() = default;
//-----------------------------------------------------------------------
Details::BuiltinShaderPool & GraphicsDevice::ShaderPool()
{
	POMDOG_ASSERT(impl);
	return impl->shaderPool;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeGraphicsDevice* GraphicsDevice::NativeGraphicsDevice()
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeGraphicsDevice);
	return impl->nativeGraphicsDevice.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
