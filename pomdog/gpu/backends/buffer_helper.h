// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {
enum class IndexFormat : u8;
enum class InputElementFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::BufferHelper {

/// Returns the byte size of a single index element for the given IndexFormat
/// (2 for UInt16, 4 for UInt32).
[[nodiscard]] u32
toIndexElementOffsetBytes(IndexFormat elementSize) noexcept;

/// Returns the byte size of a single vertex attribute in the given InputElementFormat.
[[nodiscard]] u32
toByteSize(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::BufferHelper
