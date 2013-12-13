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

#include <memory>
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"

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
	/// @brief ブレンディング係数を指定します。
	Color BlendFactor() const;

	///@~Japanese
	/// @brief RGB カラーのブレンディングの組み合わせ方法を取得します。
	BlendFunction GetColorBlendFunction() const;

	///@~Japanese
	/// @brief アルファ値のブレンディングの組み合わせ方法を取得します。
	BlendFunction GetAlphaBlendFunction() const;

	///@~Japanese
	/// @brief ソースカラーのブレンディング係数を取得します。
	Blend GetColorSourceBlend() const;
	
	///@~Japanese
	/// @brief ソースアルファのブレンディング係数を取得します。
	Blend GetAlphaSourceBlend() const;

	///@~Japanese
	/// @brief ディスティネーションカラーのブレンディング係数を取得します。
	Blend GetColorDestinationBlend() const;

	///@~Japanese
	/// @brief ディスティネーションアルファのブレンディング係数を取得します。
	Blend GetAlphaDestinationBlend() const;

	///@~Japanese
	/// @brief マルチサンプリング時の書き込み位置を取得したビットマスクです。
	std::uint32_t GetMultiSampleMask() const;

	///@~Japanese
	/// @brief マルチサンプリングテクニックとしてアルファトゥカバレッジを使用するかどうかを取得します。
	bool IsAlphaToCoverageEnable() const;

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
	/// @remarks デフォルト値です。
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
