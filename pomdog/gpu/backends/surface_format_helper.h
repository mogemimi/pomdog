// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"

namespace pomdog::gpu::detail {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format) noexcept;
};

} // namespace pomdog::gpu::detail
