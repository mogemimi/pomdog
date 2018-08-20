// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>

namespace Pomdog {

struct PresentationParameters final {
    std::int32_t BackBufferWidth;
    std::int32_t BackBufferHeight;
    std::int32_t PresentationInterval;
    std::int32_t MultiSampleCount;
    SurfaceFormat BackBufferFormat;
    DepthFormat DepthStencilFormat;
    bool IsFullScreen;
};

} // namespace Pomdog
