// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOCHANNELS_026C7FB5_7B97_42C1_B935_4CDE40E4B4F7_HPP
#define POMDOG_AUDIOCHANNELS_026C7FB5_7B97_42C1_B935_4CDE40E4B4F7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief オーディオデータのチャンネル数を示します。
enum class AudioChannels: std::uint8_t {
	///@~Japanese
	/// @brief チャンネル数1つのモノラル音声
	Mono = 1,

	///@~Japanese
	/// @brief チャンネル数2つのステレオ音声
	Stereo = 2,
};

}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCHANNELS_026C7FB5_7B97_42C1_B935_4CDE40E4B4F7_HPP)
