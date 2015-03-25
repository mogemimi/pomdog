// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP
#define POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class TextureFilter: std::uint8_t {
	Linear,

	Point,

	Anisotropic,

	LinearMipPoint,

	PointMipLinear,

	MinLinearMagPointMipLinear,

	MinLinearMagPointMipPoint,

	MinPointMagLinearMipLinear,

	MinPointMagLinearMipPoint,
};

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP)
