//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP
#define POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "RasterizerDescription.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRasterizerState;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief ラスタライザーステートです。
class POMDOG_EXPORT RasterizerState: Noncopyable
{
public:
	RasterizerState() = delete;

	RasterizerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, RasterizerDescription const& description);

	virtual ~RasterizerState();

	///@~Japanese
	/// @brief 
	/// @details CullMode::ClockwiseFace, FillMode::Solid
	static std::shared_ptr<RasterizerState>
	CreateCullClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief
	/// @remarks RasterizerState のデフォルト値です。
	/// @details CullMode::CounterClockwiseFace, FillMode::Solid
	static std::shared_ptr<RasterizerState>
	CreateCullCounterClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	///@~Japanese
	/// @brief 
	/// @details CullMode::None, FillMode::Solid
	static std::shared_ptr<RasterizerState>
	CreateCullNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeRasterizerState* GetNativeRasterizerState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeRasterizerState> nativeRasterizerState;
	RasterizerDescription description;
};

///	@}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP)
