//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CLOCK_099669CB_7F51_4B27_8468_A5FC0CBAED9B_HPP
#define POMDOG_CLOCK_099669CB_7F51_4B27_8468_A5FC0CBAED9B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include "DurationSeconds.hpp"

namespace Pomdog {
namespace Details {

class Clock
{
public:
	Clock();
	Clock(Clock const&) = delete;
	Clock(Clock &&) = default;
	
	Clock & operator=(Clock const&) = delete;
	Clock & operator=(Clock &&) = default;

	~Clock();
	
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

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_CLOCK_099669CB_7F51_4B27_8468_A5FC0CBAED9B_HPP)
