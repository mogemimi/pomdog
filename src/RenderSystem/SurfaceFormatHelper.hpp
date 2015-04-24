// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SURFACEFORMATHELPER_BDAD06C9_HPP
#define POMDOG_SURFACEFORMATHELPER_BDAD06C9_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SURFACEFORMATHELPER_BDAD06C9_HPP
