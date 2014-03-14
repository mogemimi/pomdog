//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP
#define POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include "../Config/FundamentalTypes.hpp"
#include "EffectVariable.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief エフェクト定数バッファを定義します。
class EffectConstantDescription {
public:
	std::vector<EffectVariable> Variables;
	std::string Name;
	std::uint32_t ByteSize;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP)
