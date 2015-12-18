// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP
#define POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>

namespace Pomdog {

struct PresentationParameters {
    std::int32_t BackBufferWidth;
    std::int32_t BackBufferHeight;
    std::int32_t PresentationInterval;
    std::int32_t MultiSampleCount;
    SurfaceFormat BackBufferFormat;
    DepthFormat DepthStencilFormat;
    bool IsFullScreen;
};

} // namespace Pomdog

#endif // POMDOG_PRESENTATIONPARAMETERS_2A860062_HPP
