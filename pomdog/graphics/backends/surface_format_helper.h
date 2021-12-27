// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.h"

namespace pomdog::detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format) noexcept;
};

} // namespace pomdog::detail
