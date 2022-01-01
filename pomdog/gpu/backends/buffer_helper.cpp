// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexFormat elementSize) noexcept
{
    switch (elementSize) {
    case IndexFormat::UInt16:
        return 2;
    case IndexFormat::UInt32:
        return 4;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

std::uint16_t ToByteSize(InputElementFormat format) noexcept
{
    static_assert(sizeof(float) == 4, "FUS RO DAH");
    switch (format) {
    case InputElementFormat::Byte4:
    case InputElementFormat::Float:
    case InputElementFormat::HalfFloat2:
        return 4;
    case InputElementFormat::Float2:
    case InputElementFormat::HalfFloat4:
        return 8;
    case InputElementFormat::Float3:
        return 12;
    case InputElementFormat::Int4:
    case InputElementFormat::Float4:
        return 16;
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

} // namespace pomdog::gpu::detail::BufferHelper
