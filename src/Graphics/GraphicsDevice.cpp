// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
class GraphicsDevice::Impl {
public:
	explicit Impl(std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> && device);

public:
	Detail::BuiltinShaderPool shaderPool;

	std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> nativeGraphicsDevice;
};
//-----------------------------------------------------------------------
GraphicsDevice::Impl::Impl(std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> && device)
	: nativeGraphicsDevice(std::move(device))
{}
//-----------------------------------------------------------------------
GraphicsDevice::GraphicsDevice(std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> && device)
	: impl(std::make_unique<Impl>(std::move(device)))
{}
//-----------------------------------------------------------------------
GraphicsDevice::~GraphicsDevice() = default;
//-----------------------------------------------------------------------
ShaderLanguage GraphicsDevice::GetSupportedLanguage() const
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeGraphicsDevice);
	return impl->nativeGraphicsDevice->GetSupportedLanguage();
}
//-----------------------------------------------------------------------
Detail::BuiltinShaderPool & GraphicsDevice::ShaderPool()
{
	POMDOG_ASSERT(impl);
	return impl->shaderPool;
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeGraphicsDevice* GraphicsDevice::NativeGraphicsDevice()
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeGraphicsDevice);
	return impl->nativeGraphicsDevice.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
