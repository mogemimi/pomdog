//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP
#define POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {

class ErrorCheckerAL {
public:
	static void CheckError(char const* command, char const* filename, int line);
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ERRORCHECKERAL_CD663609_0C15_464F_A972_6E33B2F595BA_HPP)
