// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ERRORCHECKER_52C9764D_HPP
#define POMDOG_ERRORCHECKER_52C9764D_HPP

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

#endif // POMDOG_ERRORCHECKER_52C9764D_HPP
