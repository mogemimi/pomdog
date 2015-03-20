//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP
#define POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeBlendState;

}// namespace RenderSystem
}// namespace Detail

///@~Japanese
/// @brief ブレンディングステートです。
class POMDOG_EXPORT BlendState {
public:
	BlendState() = delete;
	BlendState(BlendState const&) = delete;
	BlendState & operator=(BlendState const&) = delete;

	BlendState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, BlendDescription const& description);

	~BlendState();

	///@~Japanese
	/// @brief ブレンディングステートを作成するために記述したブレンディングステートの設定内容を取得します。
	BlendDescription Description() const;

	///@~Japanese
	/// @brief 加算ブレンディングを有効にするブレンディングステートです。
	static std::shared_ptr<BlendState>
	CreateAdditive(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 乗算済みアルファを使用したアルファブレンディングを有効にするブレンディングステートです。
	/// @remarks 補間アルファを使用したアルファブレンディングの場合は、CreateNonPremultiplied を使います。
	static std::shared_ptr<BlendState>
	CreateAlphaBlend(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 補間アルファを使用したアルファブレンディングを有効にするブレンディングステートです。
	static std::shared_ptr<BlendState>
	CreateNonPremultiplied(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 不透明ブレンディングを有効にするブレンディングステートです。
	/// @remarks BlendDescription の既定値を設定したステートを生成します。
	/// @details このステートはデフォルトで設定されています。
	static std::shared_ptr<BlendState>
	CreateOpaque(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
	Detail::RenderSystem::NativeBlendState* NativeBlendState();

private:
	std::unique_ptr<Detail::RenderSystem::NativeBlendState> nativeBlendState;
	BlendDescription description;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BLENDSTATE_CFC118A6_5546_4C83_9888_2399BF4E6602_HPP)
