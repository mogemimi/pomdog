// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP
#define POMDOG_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

struct ErrorChecker {
	static void CheckError(char const* command, char const* filename, int line);
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ERRORCHECKER_52C9764D_200D_48D2_8943_2813C3BE72EA_HPP)
