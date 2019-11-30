// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/GIFImage.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog::GIF {

/// Reads a GIF animation image from file (.gif).
[[nodiscard]] POMDOG_EXPORT
std::tuple<GIFImage, std::shared_ptr<Error>>
DecodeFile(const std::string& filePath);

} // namespace Pomdog::GIF
