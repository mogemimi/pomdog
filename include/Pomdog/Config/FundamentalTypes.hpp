//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_FUNDAMENTALTYPES_53FF8B0D_2C2F_4C78_AA5F_288982150FAE_HPP
#define POMDOG_FUNDAMENTALTYPES_53FF8B0D_2C2F_4C78_AA5F_288982150FAE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <cfloat>

namespace Pomdog {
namespace Details {

#if defined(_SIZE_T_DEFINED) || defined(__SIZE_TYPE__)
	using std::size_t;
#else
	typedef unsigned int size_t;
#endif

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_FUNDAMENTALTYPES_53FF8B0D_2C2F_4C78_AA5F_288982150FAE_HPP)
