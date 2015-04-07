// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP
#define POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>

namespace Pomdog {

struct PresentationParameters {
    std::uint32_t BackBufferWidth;
    std::uint32_t BackBufferHeight;
    std::uint32_t PresentationInterval;
    SurfaceFormat SurfaceFormat;
    DepthFormat DepthFormat;
    bool IsFullScreen;
};

} // namespace Pomdog

#endif // POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP
