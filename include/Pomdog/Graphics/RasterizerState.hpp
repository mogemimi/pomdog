//
//  Copyright (C) 2013-2014 mogemimi.
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
#include "RasterizerDescription.hpp"
#include "Pomdog/Config/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRasterizerState;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief ラスタライザステートです。
class POMDOG_EXPORT RasterizerState {
public:
	RasterizerState() = delete;
	RasterizerState(RasterizerState const&) = delete;
	RasterizerState & operator=(RasterizerState const&) = delete;

	RasterizerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, RasterizerDescription const& description);

	~RasterizerState();

	///@~Japanese
	/// @brief ラスタライザステートを作成するために記述したラスタライザステートの設定内容を取得します。
	RasterizerDescription Description() const;

	///@~Japanese
	/// @brief ワインディング順序が時計回りの頂点列に対して背面カリングを有効にするラスタライザステートを作成します。
	/// @details
	/// RasterizerDescription desc;
	/// desc.CullMode = CullMode::ClockwiseFace;
	/// desc.FillMode = FillMode::Solid;
	static std::shared_ptr<RasterizerState>
	CreateCullClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief ワインディング順序が反時計回りの頂点列に対して背面カリングを有効にするラスタライザステートを作成します。
	/// @remarks RasterizerState のデフォルト値です。
	/// @details
	/// RasterizerDescription desc;
	/// desc.CullMode = CullMode::CounterClockwiseFace;
	/// desc.FillMode = FillMode::Solid;
	static std::shared_ptr<RasterizerState>
	CreateCullCounterClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	///@~Japanese
	/// @brief カリングを無効化する設定を持つラスタライザステートを作成します。
	/// @details
	/// RasterizerDescription desc;
	/// desc.CullMode = CullMode::None;
	/// desc.FillMode = FillMode::Solid;
	static std::shared_ptr<RasterizerState>
	CreateCullNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeRasterizerState* NativeRasterizerState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeRasterizerState> nativeRasterizerState;
	RasterizerDescription description;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP)
