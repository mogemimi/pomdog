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
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Color;
class ImageContainer;
} // namespace pomdog

namespace pomdog {

enum class PNMEncoding : i8 {
    ASCII,
    Binary,
};

enum class PNMSubtype : i8 {
    Bitmap,
    Graymap,
    Pixmap,
};

struct PNMEncodeOptions final {
    PNMSubtype subtype = {};
    PNMEncoding encoding = {};
    u8 maxValue = 0;
};

/// Reads a PNM/Netpbm image from data (.pbm .pgm .ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageContainer, std::unique_ptr<Error>>
decodePNM(const char* data, std::size_t size);

/// Writes an image to a given buffer in a P6 format (.ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::vector<u8>, std::unique_ptr<Error>>
encodePNM(const Color* pixelData, std::size_t size, int width, int height);

/// Writes an image to a given buffer in a PNM/Netpbm format (.pbm .pgm .ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::vector<u8>, std::unique_ptr<Error>>
encodePNM(const Color* pixelData, std::size_t size, int width, int height, const PNMEncodeOptions& options);

} // namespace pomdog
