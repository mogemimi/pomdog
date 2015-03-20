// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSDEVICE_7A54711C_54D1_4067_AD81_252DC98015CE_HPP
#define POMDOG_GRAPHICSDEVICE_7A54711C_54D1_4067_AD81_252DC98015CE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {

class BuiltinShaderPool;

namespace RenderSystem {

class NativeGraphicsDevice;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT GraphicsDevice {
public:
	GraphicsDevice() = delete;
	GraphicsDevice(GraphicsDevice const&) = delete;
	GraphicsDevice & operator=(GraphicsDevice const&) = delete;

	explicit GraphicsDevice(std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> && nativeDevice);

	~GraphicsDevice();

	ShaderLanguage GetSupportedLanguage() const;

	Detail::BuiltinShaderPool & ShaderPool();

public:
	///@brief internal method
	Detail::RenderSystem::NativeGraphicsDevice* NativeGraphicsDevice();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSDEVICE_7A54711C_54D1_4067_AD81_252DC98015CE_HPP)
