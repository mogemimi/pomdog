// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format);
};

} // namespace Detail
} // namespace Pomdog
