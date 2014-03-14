//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP
#define POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <bitset>
#include <limits>
#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

enum class KeyState: bool;
enum class Keys: std::uint8_t;

/// @addtogroup Framework
/// @{
/// @addtogroup Input
/// @{

/// @brief キーボードの状態です。
class POMDOG_EXPORT KeyboardState {
public:
	KeyboardState() = default;
	~KeyboardState() = default;

	///@~Japanese
	/// @brief 指定されたキーの現在の状態を取得します。
	KeyState operator[](Keys key) const;

public:
	///@~Japanese
	/// @brief 指定されたキーの状態を KeyState::Pressed に設定します。
	void AddPressedKey(Keys key);
	
private:
	std::bitset<std::numeric_limits<std::uint8_t>::max()> keyset;
};


/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP)
