//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHSTENCILSTATE_8D3E62B6_0EDC_4443_BB95_C5ECBD493B38_HPP
#define POMDOG_DEPTHSTENCILSTATE_8D3E62B6_0EDC_4443_BB95_C5ECBD493B38_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "DepthStencilDescription.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDepthStencilState;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 深度ステンシルステートを定義します。
class POMDOG_EXPORT DepthStencilState: Noncopyable
{
public:
	DepthStencilState() = delete;

	DepthStencilState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, DepthStencilDescription const& description);

	~DepthStencilState();

	///@~Japanese
	/// @brief 深度ステンシルステートを作成するために記述した深度ステンシルステートの設定内容を取得します。
	DepthStencilDescription Description() const;

	///@~Japanese
	/// @brief 深度ステンシルバッファを有効化するステートを作成します。
	/// @remarks DepthStencilDescription の既定値を設定したステートを生成します。
	/// @details このステートはデフォルトで設定されています。
	static std::shared_ptr<DepthStencilState>
	CreateReadWriteDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 読み取り専用の深度ステンシルバッファを有効化するステートを作成します。
	static std::shared_ptr<DepthStencilState>
	CreateReadOnlyDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 深度ステンシルバッファを使用しないステートを作成します。
	static std::shared_ptr<DepthStencilState>
	CreateNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeDepthStencilState* NativeDepthStencilState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeDepthStencilState> nativeDepthStencilState;
	DepthStencilDescription description;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILSTATE_8D3E62B6_0EDC_4443_BB95_C5ECBD493B38_HPP)
