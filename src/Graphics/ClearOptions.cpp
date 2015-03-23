// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ClearOptions.hpp"

namespace Pomdog {

#if __cplusplus < 201402L
ClearOptions & operator|=(ClearOptions & a, ClearOptions b)
{
	a = a | b;
	return a;
}
#endif

}// namespace Pomdog
