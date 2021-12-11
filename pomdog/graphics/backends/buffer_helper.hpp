// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <cstdlib>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept;

std::uint16_t ToByteSize(InputElementFormat format) noexcept;

} // namespace pomdog::detail::BufferHelper