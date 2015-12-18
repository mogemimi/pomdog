// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_ERRORCHECKERAL_CD663609_HPP
#define POMDOG_ERRORCHECKERAL_CD663609_HPP

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ErrorCheckerAL {
public:
    static void CheckError(char const* command, char const* filename, int line);
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_ERRORCHECKERAL_CD663609_HPP
