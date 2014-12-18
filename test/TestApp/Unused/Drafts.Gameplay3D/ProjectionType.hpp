//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PROJECTIONTYPE_E6EFA410_142A_44CA_BF7B_56A4EC7BC23A_HPP
#define POMDOG_PROJECTIONTYPE_E6EFA410_142A_44CA_BF7B_56A4EC7BC23A_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class ProjectionType: std::uint8_t
{
	///@~Japanese
	/// @brief 平行投影を示します。
	Orthographic,

	///@~Japanese
	/// @brief 透視投影を示します。
	Perspective,
};

}// namespace Pomdog

#endif // !defined(POMDOG_PROJECTIONTYPE_E6EFA410_142A_44CA_BF7B_56A4EC7BC23A_HPP)
