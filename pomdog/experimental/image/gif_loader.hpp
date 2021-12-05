// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/GIFImage.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GIF {

/// Reads a GIF animation image from file (.gif).
[[nodiscard]] POMDOG_EXPORT std::tuple<GIFImage, std::unique_ptr<Error>>
DecodeFile(const std::string& filePath);

} // namespace Pomdog::GIF
