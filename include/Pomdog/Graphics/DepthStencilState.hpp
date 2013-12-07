//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP
#define POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Utility/Noncopyable.hpp"
#include "DepthStencilDescription.hpp"
#include <memory>

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
class DepthStencilState: Noncopyable
{
public:
	DepthStencilState() = delete;

	DepthStencilState(std::shared_ptr<GraphicsDevice> const& device, DepthStencilDescription const& description);

	virtual ~DepthStencilState() = default;

	///@~Japanese
	/// @brief 深度ステンシルバッファを有効化するステートを作成します。
	/// @remarks 既定値に設定されたステートを生成します。
	/// @details このステートはデフォルトで設定されています。
	static std::shared_ptr<DepthStencilState> CreateReadWriteDepth(GraphicsDevice& device);

	///@~Japanese
	/// @brief 読み取り専用の深度ステンシルバッファを有効化するステートを作成します。
	static std::shared_ptr<DepthStencilState> CreateReadOnlyDepth(GraphicsDevice& device);

	///@~Japanese
	/// @brief 深度ステンシルバッファを使用しないステートを作成します。
	static std::shared_ptr<DepthStencilState> CreateNone(GraphicsDevice& device);
	
public:
	Details::RenderSystem::NativeDepthStencilState* GetNativeDepthStencilState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeDepthStencilState> nativeState;
	DepthStencilDescription description;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP)
