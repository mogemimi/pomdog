// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {
enum class IndexFormat : u8;
enum class InputElementFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::BufferHelper {

[[nodiscard]] u32
toIndexElementOffsetBytes(IndexFormat elementSize) noexcept;

[[nodiscard]] u16
ToByteSize(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::BufferHelper
