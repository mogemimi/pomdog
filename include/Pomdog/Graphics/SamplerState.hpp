//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP
#define POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "SamplerDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeSamplerState;

}// namespace RenderSystem
}// namespace Details

class GraphicsDevice;

///@~Japanese
/// @brief テクスチャのサンプリング方法を指定するサンプラーステートです。
class POMDOG_EXPORT SamplerState {
public:
	SamplerState() = delete;
	SamplerState(SamplerState const&) = delete;
	SamplerState & operator=(SamplerState const&) = delete;

	SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, SamplerDescription const& description);

	~SamplerState();

	///@~Japanese
	/// @brief サンプラーステートを作成するために記述したサンプラーステートの設定内容を取得します。
	SamplerDescription Description() const;

	///@~Japanese
	/// @brief 異方性フィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Clamp を使用します。
	static std::shared_ptr<SamplerState>
	CreateAnisotropicClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 異方性フィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Wrap を使用します。
	static std::shared_ptr<SamplerState>
	CreateAnisotropicWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 線形フィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Clamp を使用します。
	static std::shared_ptr<SamplerState>
	CreateLinearClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 線形フィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Wrap を使用します。
	static std::shared_ptr<SamplerState>
	CreateLinearWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief ポイントフィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Clamp を使用します。
	static std::shared_ptr<SamplerState>
	CreatePointClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief ポイントフィルタリングを有効にするサンプラステートを作成します。
	/// 範囲外テクスチャ座標のアドレッシング方法は TextureAddressMode::Wrap を使用します。
	static std::shared_ptr<SamplerState>
	CreatePointWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
	Details::RenderSystem::NativeSamplerState* NativeSamplerState();

private:
	std::unique_ptr<Details::RenderSystem::NativeSamplerState> nativeSamplerState;
	SamplerDescription description;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SAMPLERSTATE_87568CD5_37AB_4449_B4A1_3AB24A59A849_HPP)
