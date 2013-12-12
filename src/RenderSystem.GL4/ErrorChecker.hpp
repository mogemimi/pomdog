//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP
#define POMDOG_SRC_GL4_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class ErrorChecker
{
public:
	static void CheckError(char const* command, char const* filename, int line);
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP)
