//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP
#define POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "BlendDescription.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeBlendState;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
///	@{

///@~Japanese
/// @brief カラーブレンディングステートです。
class POMDOG_EXPORT BlendState: Noncopyable
{
public:
	BlendState() = delete;

	BlendState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, BlendDescription const& description);

	virtual ~BlendState();

	///@~Japanese
	/// @brief
	static std::shared_ptr<BlendState>
	CreateAdditive(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	static std::shared_ptr<BlendState>
	CreateAlphaBlend(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 
	static std::shared_ptr<BlendState>
	CreateNonPremultiplied(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 
	/// @details デフォルトです。
	static std::shared_ptr<BlendState>
	CreateOpaque(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeBlendState* GetNativeBlendState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeBlendState> nativeBlendState;
	BlendDescription description;
};

///	@}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP)
