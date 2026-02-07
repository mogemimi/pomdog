// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class ImageContainer;
} // namespace pomdog

namespace pomdog::DDS {

/// Reads a DDS image from data (.dds).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageContainer, std::unique_ptr<Error>>
decode(const u8* data, std::size_t size);

} // namespace pomdog::DDS
