//
//  Copyright (C) 2013 mogemimi.
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
#include "detail/ForwardDeclarations.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

using EffectParameterCollection = std::unordered_map<std::string, std::shared_ptr<EffectParameter>>;

class EffectPass
{
public:
	~EffectPass() = default;

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
	
private:
	EffectParameterCollection effectParameters;
};
	
/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP)
