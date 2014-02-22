//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP
#define POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <unordered_map>
#include <string>
#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {

class NativeEffectPass;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

using EffectParameterCollection = std::unordered_map<std::string, std::shared_ptr<EffectParameter>>;

class POMDOG_EXPORT EffectPass: public std::enable_shared_from_this<EffectPass>
{
public:
	EffectPass() = delete;
	EffectPass(EffectPass const&) = delete;
	EffectPass(EffectPass &&) = default;
	
	EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::shared_ptr<GraphicsContext> const& graphicsContext,
		Details::ShaderBytecode const& vertexShader,
		Details::ShaderBytecode const& pixelShader);

	~EffectPass();
	
	EffectPass & operator=(EffectPass const&) = delete;
	EffectPass & operator=(EffectPass &&) = default;

	///@~Japanese
	/// @brief エフェクトを適用します。
	/// @remarks EffectParameter の値を変更した場合、
	/// 変更を適用するために変更後にこのメソッドを呼び出す必要があります。
	void Apply();

	///@~Japanese
	/// @brief エフェクトパラメータを取得します。
	std::shared_ptr<EffectParameter> const& Parameters(std::string const& parameterName) const;
	
	///@~Japanese
	/// @brief エフェクトパラメータのコンテナを取得します。
	EffectParameterCollection const& Parameters() const;
	
public:
	Details::RenderSystem::NativeEffectPass* NativeEffectPass();
	
private:
	EffectParameterCollection effectParameters;
	std::weak_ptr<GraphicsContext> graphicsContext;
	std::unique_ptr<Details::RenderSystem::NativeEffectPass> nativeEffectPass;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP)
