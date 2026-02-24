// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::SurfaceFormatHelper {

/// Returns the number of bytes per block for the specified pixel format.
///
/// For BCn (block compression) formats, this returns the byte size of a single
/// 4x4 texel block (e.g. 8 bytes for BC1, 16 bytes for BC2/BC3), not the size
/// of a single pixel. When computing row pitch or image size for BCn textures,
/// use block dimensions (i.e. ceil(width/4) and ceil(height/4)) instead of
/// pixel dimensions.
[[nodiscard]] i32
toBytesPerBlock(PixelFormat format) noexcept;

} // namespace pomdog::gpu::detail::SurfaceFormatHelper
