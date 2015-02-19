//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BEAMEMITTER_89CB54AB_FD79_45B7_BAF1_EF7FC1F748B2_HPP
#define POMDOG_BEAMEMITTER_89CB54AB_FD79_45B7_BAF1_EF7FC1F748B2_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Application/DurationSeconds.hpp"
#include "Pomdog/Math/Color.hpp"
#include <cstdint>
#include <random>

namespace Pomdog {

class BeamEmitter {
public:
	DurationSeconds Duration {5.0};
	bool Looping = true;

	Color StartColor = Color::White;
	Color EndColor = Color::White;

	///@note [0, std::numelic_limits<T>::max()]
	std::uint32_t InterpolationPoints = 90;

	///@note 線の凹凸（ぎざぎざ）の振れ幅。
	std::uniform_real_distribution<float> SwayRange {-8.0f, 8.0f};

	///@brief 枝の広がり。
	///@remarks 光線をターゲットに集める場合は 0 を指定します。
	float ShapeWidth = 0;

	///@brief 線の濃さ。
	float StartThickness = 1.0f;

	///@note [0.0, 5.0]
	/// この値が 0 に近づくほどスムーズに、大きくなるほどでこぼこになります。
	float Jaggedness = 0.7f;

	float StartLifetime = 1;

	///@brief 発生させるビームの最大数
	std::uint16_t MaxBeams = 16;

	///@brief 1 秒間に発生させるビームの数
	std::uint16_t EmissionRate = 4;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAMEMITTER_89CB54AB_FD79_45B7_BAF1_EF7FC1F748B2_HPP)
