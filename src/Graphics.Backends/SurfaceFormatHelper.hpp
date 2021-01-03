// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"

namespace Pomdog::Detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format) noexcept;
};

} // namespace Pomdog::Detail
