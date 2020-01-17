// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog::Detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format);
};

} // namespace Pomdog::Detail
