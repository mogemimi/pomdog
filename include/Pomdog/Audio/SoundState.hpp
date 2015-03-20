// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP
#define POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief サウンドの現在の状態を定義します。
enum class SoundState: std::uint8_t {
	///@~Japanese
	/// @brief 一時停止しています。
	Paused,

	///@~Japanese
	/// @brief 再生しています。
	Playing,

	///@~Japanese
	/// @brief 停止しています。
	Stopped,
};

}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP)
