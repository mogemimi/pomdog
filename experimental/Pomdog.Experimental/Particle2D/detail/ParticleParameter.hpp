//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLEPARAMETER_FCCEE8BC_68A3_4A5B_8075_ED0E0170C937_HPP
#define POMDOG_PARTICLEPARAMETER_FCCEE8BC_68A3_4A5B_8075_ED0E0170C937_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <random>

namespace Pomdog {
namespace Details {
namespace Particles {

template <typename T>
class ParticleParameter {
public:
	virtual ~ParticleParameter() = default;

	virtual T Compute(float normalizedScale, std::mt19937 & random) const = 0;

	virtual T Compute(float normalizedScale, float amount) const = 0;

	virtual float GenerateVariance(std::mt19937 & random) const = 0;
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETER_FCCEE8BC_68A3_4A5B_8075_ED0E0170C937_HPP)
