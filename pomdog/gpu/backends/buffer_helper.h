// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"

namespace pomdog::gpu::detail::BufferHelper {

[[nodiscard]] u32
toIndexElementOffsetBytes(IndexFormat elementSize) noexcept;

[[nodiscard]] u16
ToByteSize(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::BufferHelper
