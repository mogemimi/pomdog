// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP
#define POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ErrorCheckerAL {
public:
	static void CheckError(char const* command, char const* filename, int line);
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP)
