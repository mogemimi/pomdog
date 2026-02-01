// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::hash {

/// Computes the CRC-32 checksum for the given data buffer.
[[nodiscard]] POMDOG_EXPORT u32
computeCRC32(std::span<const u8> data) noexcept;

/// Computes the CRC-32 checksum for the given string view.
[[nodiscard]] POMDOG_EXPORT u32
computeCRC32(std::string_view data) noexcept;

} // namespace pomdog::hash
