// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"

namespace pomdog::gpu::detail::SurfaceFormatHelper {

[[nodiscard]] int toBytesPerBlock(PixelFormat format) noexcept;

} // namespace pomdog::gpu::detail::SurfaceFormatHelper
