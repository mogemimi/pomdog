//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP
#define POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <bitset>
#include <limits>

namespace Pomdog {

enum class KeyState: bool;
enum class Keys: std::uint8_t;

/// @brief キーボードの状態です。
class POMDOG_EXPORT KeyboardState {
public:
	///@~Japanese
	/// @brief 指定されたキーの現在の状態を取得します。
	KeyState operator[](Keys key) const;

	///@~Japanese
	/// @brief 指定されたキーが押されているかどうかを取得します。
	bool IsKeyDown(Keys key) const;
	
	///@~Japanese
	/// @brief 指定されたキーが離されているかどうかを取得します。
	bool IsKeyUp(Keys key) const;

	void SetKey(Keys key, KeyState keyState);

	void ClearAllKeys();

private:
	std::bitset<std::numeric_limits<std::uint8_t>::max()> keyset;
};

}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDSTATE_8518E1F7_25E7_4E90_AD55_055740A9EA0E_HPP)
