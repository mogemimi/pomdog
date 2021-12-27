// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/image/gif_image.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::GIF {

/// Reads a GIF animation image from file (.gif).
[[nodiscard]] POMDOG_EXPORT std::tuple<GIFImage, std::unique_ptr<Error>>
DecodeFile(const std::string& filePath);

} // namespace pomdog::GIF
