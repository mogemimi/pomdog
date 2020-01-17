// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Image/ImageBuffer.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>

namespace Pomdog::PNG {

/// Reads a PNG image from data (.png).
[[nodiscard]] POMDOG_EXPORT
std::tuple<ImageBuffer, std::shared_ptr<Error>>
Decode(const std::uint8_t* data, std::size_t size);

} // namespace Pomdog::PNG
