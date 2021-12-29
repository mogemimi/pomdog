// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <cstdlib>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept;

std::uint16_t ToByteSize(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::BufferHelper
