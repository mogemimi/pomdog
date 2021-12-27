// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/backends/buffer_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/graphics/index_element_size.h"
#include "pomdog/graphics/input_element_format.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::ThirtyTwoBits:
        return 4;
    case IndexElementSize::SixteenBits:
        return 2;
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

} // namespace pomdog::detail::BufferHelper
