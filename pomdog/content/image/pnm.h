// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/image/image_buffer.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::PNM {

enum class PNMEncoding : std::int8_t {
    ASCII,
    Binary,
};

enum class PNMSubtype : std::int8_t {
    Bitmap,
    Graymap,
    Pixmap,
};

struct PNMEncodeOptions final {
    PNMSubtype Subtype;
    PNMEncoding Encoding;
    std::uint8_t MaxValue;
};

/// Reads a PNM/Netpbm image from data (.pbm .pgm .ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageBuffer, std::unique_ptr<Error>>
decode(const char* data, std::size_t size);

/// Writes an image to a given buffer in a P6 format (.ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::vector<std::uint8_t>, std::unique_ptr<Error>>
encode(const Color* pixelData, std::size_t size, int width, int height);

/// Writes an image to a given buffer in a PNM/Netpbm format (.pbm .pgm .ppm).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::vector<std::uint8_t>, std::unique_ptr<Error>>
encode(const Color* pixelData, std::size_t size, int width, int height, const PNMEncodeOptions& options);

} // namespace pomdog::PNM
