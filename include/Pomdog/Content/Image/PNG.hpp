// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Image/ImageBuffer.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::PNG {

/// Reads a PNG image from data (.png).
[[nodiscard]] POMDOG_EXPORT
std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(const std::uint8_t* data, std::size_t size);

} // namespace Pomdog::PNG
