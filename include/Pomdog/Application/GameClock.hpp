//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP
#define POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Application/DurationSeconds.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GameClock {
public:
	GameClock();
	GameClock(GameClock const&) = delete;
	GameClock(GameClock &&) = default;
	
	GameClock & operator=(GameClock const&) = delete;
	GameClock & operator=(GameClock &&) = default;

	~GameClock();
	
	///@~Japanese
	/// @brief フレーム毎に 1 度だけ呼び出し、フレーム時間の更新やデルタ時間の予測を行います。
	void Tick();
	
	///@~Japanese
	/// @brief アプリケーションが起動してから現在のフレームまでの経過時間（秒）を取得します。
	DurationSeconds TotalGameTime() const;
	
	///@~Japanese
	/// @brief 現在のフレーム番号を取得します。
	std::uint32_t FrameNumber() const;

	///@~Japanese
	/// @brief 現在のフレームにかかる処理時間の予測値を取得します。
	DurationSeconds FrameDuration() const;

	///@~Japanese
	/// @brief 現在のフレームレートを取得します。
	float FrameRate() const;

	///@~Japanese
	/// @brief フレームの開始時から現在までの経過時間（秒）を取得します。
	DurationSeconds ElapsedTime() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP)
