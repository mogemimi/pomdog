//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP
#define POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeConstantLayout;

}// namespace RenderSystem
}// namespace Details

using EffectParameterCollection = std::unordered_map<std::string, std::shared_ptr<EffectParameter>>;

class POMDOG_EXPORT ConstantBufferBinding {
public:
	ConstantBufferBinding() = delete;
	ConstantBufferBinding(ConstantBufferBinding const&) = delete;
	ConstantBufferBinding(ConstantBufferBinding &&) = default;
	
	ConstantBufferBinding(GraphicsDevice & graphicsDevice, EffectPass & effectPass);
	
	ConstantBufferBinding(std::shared_ptr<GraphicsDevice> const& graphicsDevice, EffectPass & effectPass);

	~ConstantBufferBinding();
	
	ConstantBufferBinding & operator=(ConstantBufferBinding const&) = delete;
	ConstantBufferBinding & operator=(ConstantBufferBinding &&) = default;

	///@~Japanese
	/// @brief エフェクトパラメータを取得します。
	std::shared_ptr<EffectParameter> const& Find(std::string const& parameterName) const;
	
	///@~Japanese
	/// @brief エフェクトパラメータのコンテナを取得します。
	EffectParameterCollection const& Find() const;
	
public:
	Details::RenderSystem::NativeConstantLayout* NativeConstantLayout();
	
private:
	EffectParameterCollection effectParameters;
	std::unique_ptr<Details::RenderSystem::NativeConstantLayout> nativeConstantLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP)
