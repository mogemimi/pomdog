//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GRAPHICSDEVICE_69C4893A_5F54_11E3_BC28_A8206655A22B_HPP
#define POMDOG_GRAPHICSDEVICE_69C4893A_5F54_11E3_BC28_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Utility/Noncopyable.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsDevice;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

class GraphicsDevice: Noncopyable
{
public:
	GraphicsDevice() = delete;
	
	explicit GraphicsDevice(std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> nativeDevice);
	
	virtual ~GraphicsDevice();
	
public:
	///@brief internal method
	Details::RenderSystem::NativeGraphicsDevice* GetNativeGraphicsDevice();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeGraphicsDevice> nativeDevice;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSDEVICE_69C4893A_5F54_11E3_BC28_A8206655A22B_HPP)
