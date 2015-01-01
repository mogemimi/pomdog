//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_GAMEPADCAPABILITIES_869064F7_A425_45C1_A614_8B7DC755658F_HPP
#define POMDOG_GAMEPADCAPABILITIES_869064F7_A425_45C1_A614_8B7DC755658F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "GamepadType.hpp"
#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief ゲームパッドの能力を示します。
class GamepadCapabilities {
public:
	///@~Japanese
	/// @brief ゲームパッドの種類を表します。
	GamepadType GamepadType = GamepadType::Unknown;

	///@~Japanese
	/// @brief ゲームパッドが接続されているかどうか取得します。
	/// @remarks 接続されている場合は true を、それ以外は false をとります。
	bool IsConnected = false;

	///@~Japanese
	/// @brief 左サムスティックの水平移動が有効かどうか取得します。
	/// @remarks 有効である場合は true を、それ以外は false をとります。
	bool HasLeftXThumbStick = false;

	///@~Japanese
	/// @brief 左サムスティックの垂直移動が有効かどうか取得します。
	/// @remarks 有効である場合は true を、それ以外は false をとります。
	bool HasLeftYThumbStick = false;

	///@~Japanese
	/// @brief 右サムスティックの水平移動が有効かどうか取得します。
	/// @remarks 有効である場合は true を、それ以外は false をとります。
	bool HasRightXThumbStick = false;

	///@~Japanese
	/// @brief 右サムスティックの垂直移動が有効かどうか取得します。
	/// @remarks 有効である場合は true を、それ以外は false をとります。
	bool HasRightYThumbStick = false;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEPADCAPABILITIES_869064F7_A425_45C1_A614_8B7DC755658F_HPP)
