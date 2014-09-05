//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEEMITTERSHAPE_DCE4AD88_E068_4AF6_8F1F_EEE13290929E_HPP
#define POMDOG_PARTICLEEMITTERSHAPE_DCE4AD88_E068_4AF6_8F1F_EEE13290929E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <random>

namespace Pomdog {
namespace Details {
namespace Particles {

class ParticleEmitterShape {
public:
	virtual ~ParticleEmitterShape() = default;
	
	virtual void Compute(std::mt19937 & random, Vector2 & emitPosition, Radian<float> & emitAngle) const = 0;
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTERSHAPE_DCE4AD88_E068_4AF6_8F1F_EEE13290929E_HPP)
