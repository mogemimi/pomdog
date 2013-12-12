//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP
#define POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "SamplerDescription.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeSamplerState;

}// namespace RenderSystem
}// namespace Details

class GraphicsDevice;

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief テクスチャのサンプリング方法を指定するサンプラーステートです。
class POMDOG_EXPORT SamplerState: Noncopyable
{
public:
	SamplerState() = delete;

	SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, SamplerDescription const& description);
	
	virtual ~SamplerState();

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreateAnisotropicClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreateAnisotropicWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreateLinearClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreateLinearWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreatePointClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<SamplerState>
	CreatePointWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeSamplerState* GetNativeSamplerState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeSamplerState> nativeSamplerState;
	SamplerDescription description;
};

///	@}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP)
