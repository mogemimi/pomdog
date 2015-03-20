// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLECURVEKEY_6EBF4743_2FC9_43B9_9760_0FE25327F8F4_HPP
#define POMDOG_PARTICLECURVEKEY_6EBF4743_2FC9_43B9_9760_0FE25327F8F4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
struct ParticleCurveKey {
	//std::uint8_t NormalizedTime;
	float TimeSeconds;
	T Value;
};

}// namespace Particles
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLECURVEKEY_6EBF4743_2FC9_43B9_9760_0FE25327F8F4_HPP)
