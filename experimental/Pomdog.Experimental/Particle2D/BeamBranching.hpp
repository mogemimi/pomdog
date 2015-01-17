//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BEAMBRANCHING_1888F4E0_8B38_413D_B9ED_260B011E70D4_HPP
#define POMDOG_BEAMBRANCHING_1888F4E0_8B38_413D_B9ED_260B011E70D4_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>
#include <random>

namespace Pomdog {

class BeamBranching {
public:
	///@brief 枝の広がり。
	std::uniform_real_distribution<float> SpreadRange {-5.0f, 5.0f};

	///@brief 枝を生成する確率を 0.0 ~ 1.0 の範囲で指定します。
	///@remarks 枝を作らない場合は 0 を、必ず枝を作る場合は 1 を指定します。
	float BranchingRate = 0.7f;

	///@brief 光線から分岐した枝の線の濃さを決めるスケール値。
	///@note 分岐元の光線の濃さ parentBeam.Thickness に対して、枝の濃さ childBeam.Thickness は
	/// childBeam.Thickness = InheritThickness * parentBeam.Thickness;
	float InheritThickness = 0.7f;

	///@brief 枝の本数。
	///@remarks 枝を作らない光線の場合は、0 を指定します。
	std::uint8_t MaxBranches = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAMBRANCHING_1888F4E0_8B38_413D_B9ED_260B011E70D4_HPP)
