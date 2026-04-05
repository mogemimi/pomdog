// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::BufferHelper {

[[nodiscard]] u32
toIndexElementOffsetBytes(IndexFormat elementSize) noexcept
{
    switch (elementSize) {
    case IndexFormat::UInt16:
        return 2;
    case IndexFormat::UInt32:
        return 4;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

[[nodiscard]] u32
toByteSize(InputElementFormat format) noexcept
{
    static_assert(sizeof(i32) == 4);
    static_assert(sizeof(f32) == 4);

    switch (format) {
    case InputElementFormat::Uint8x1:
    case InputElementFormat::Unorm8x1:
        return 1;
    case InputElementFormat::Uint8x2:
    case InputElementFormat::Unorm8x2:
        return 2;
    case InputElementFormat::Uint8x4:
    case InputElementFormat::Unorm8x4:
        return 4;
    case InputElementFormat::Float16x2:
        return 4;
    case InputElementFormat::Float16x4:
        return 8;
    case InputElementFormat::Float32x1:
    case InputElementFormat::Int32x1:
        return 4;
    case InputElementFormat::Float32x2:
    case InputElementFormat::Int32x2:
        return 8;
    case InputElementFormat::Float32x3:
    case InputElementFormat::Int32x3:
        return 12;
    case InputElementFormat::Float32x4:
    case InputElementFormat::Int32x4:
        return 16;
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

} // namespace pomdog::gpu::detail::BufferHelper
