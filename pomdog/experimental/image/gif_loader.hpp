// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/image/gif_image.hpp"
#include "pomdog/utility/errors.hpp"

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
