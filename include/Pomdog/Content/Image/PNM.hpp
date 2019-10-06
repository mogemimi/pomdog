// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Image/ImageBuffer.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

namespace Pomdog::PNM {

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
[[nodiscard]] POMDOG_EXPORT
std::tuple<ImageBuffer, std::shared_ptr<Error>>
Decode(const char* data, std::size_t size);

/// Writes an image to a given buffer in a P6 format (.ppm).
[[nodiscard]] POMDOG_EXPORT
std::tuple<std::vector<std::uint8_t>, std::shared_ptr<Error>>
Encode(const Color* pixelData, std::size_t size, int width, int height);

/// Writes an image to a given buffer in a PNM/Netpbm format (.pbm .pgm .ppm).
[[nodiscard]] POMDOG_EXPORT
std::tuple<std::vector<std::uint8_t>, std::shared_ptr<Error>>
Encode(const Color* pixelData, std::size_t size, int width, int height, const PNMEncodeOptions& options);

} // namespace Pomdog::PNM
