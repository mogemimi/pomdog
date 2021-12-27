// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/content/image/image_buffer.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::DDS {

/// Reads a DDS image from data (.dds).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(const std::uint8_t* data, std::size_t size);

} // namespace pomdog::DDS
