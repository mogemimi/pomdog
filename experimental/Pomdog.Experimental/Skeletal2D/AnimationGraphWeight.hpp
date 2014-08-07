//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONGRAPHWEIGHT_2B7D7365_F1DB_4D21_A939_F9E06945754A_HPP
#define POMDOG_ANIMATIONGRAPHWEIGHT_2B7D7365_F1DB_4D21_A939_F9E06945754A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationBlendInputType.hpp"
#include <cstdint>

namespace Pomdog {

class AnimationGraphWeight {
public:
	explicit AnimationGraphWeight(float value);

	explicit AnimationGraphWeight(bool value);

	void SetValue(float value);
	void SetValue(bool value);

	float GetFloat() const;
	bool GetBool() const;

private:
	float weight;
	AnimationBlendInputType const type;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONGRAPHWEIGHT_2B7D7365_F1DB_4D21_A939_F9E06945754A_HPP)
