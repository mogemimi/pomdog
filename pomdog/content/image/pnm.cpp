// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/image/pnm.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <charconv>
#include <cstring>
#include <limits>
#include <string_view>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

template <class StringIterator>
[[nodiscard]] std::string_view
getWord(StringIterator& it, StringIterator end)
{
    std::size_t count = 0;

    auto begin = it;
    for (; it != end; ++it) {
        const auto c = *it;
        if (c == '\r') {
            auto next = std::next(it);
            if ((next != end) && (*next == '\n')) {
                it = next;
            }
            break;
        }
        if ((c == ' ') || (c == '\n')) {
            break;
        }
        ++count;
    }
    return std::string_view{&(*begin), count};
}

template <class StringIterator>
void skipCommentLine(StringIterator& it, StringIterator end)
{
    if ((it == end) || (*it != '#')) {
        return;
    }

    for (; it != end; ++it) {
        if ((*it == '\n') && (it != end)) {
            ++it;
            if ((it == end) || (*it != '#')) {
                return;
            }
        }
    }
}

} // namespace

[[nodiscard]] std::tuple<ImageContainer, std::unique_ptr<Error>>
decodePNM(const char* data, std::size_t size)
{
    ImageContainer image = {};
    image.mipmapCount = 0;

    if (size < 7) {
        return std::make_tuple(std::move(image), errors::make("The PNM data size is too small"));
    }

    const std::string_view view{data, size};

    PNMEncoding pnmEncoding = PNMEncoding::ASCII;
    PNMSubtype pnmSubtype = PNMSubtype::Bitmap;
    int maxLuma = 1;

    auto iter = std::begin(view);

    skipCommentLine(iter, std::end(view));

    if (auto prefix = getWord(iter, std::end(view)); prefix == "P1") {
        // NOTE: Portable BitMap (.pbm)
        pnmEncoding = PNMEncoding::ASCII;
        pnmSubtype = PNMSubtype::Bitmap;
        maxLuma = 1;
    }
    else if (prefix == "P2") {
        // NOTE: Portable GrayMap (.pgm)
        pnmEncoding = PNMEncoding::ASCII;
        pnmSubtype = PNMSubtype::Graymap;
        maxLuma = 255;
    }
    else if (prefix == "P3") {
        // NOTE: Portable PixMap (.ppm)
        pnmEncoding = PNMEncoding::ASCII;
        pnmSubtype = PNMSubtype::Pixmap;
        maxLuma = 255;
    }
    else if (prefix == "P4") {
        // NOTE: Portable BitMap (.pbm)
        pnmEncoding = PNMEncoding::Binary;
        pnmSubtype = PNMSubtype::Bitmap;
        maxLuma = 255;
    }
    else if (prefix == "P5") {
        // NOTE: Portable GrayMap (.pgm)
        pnmEncoding = PNMEncoding::Binary;
        pnmSubtype = PNMSubtype::Graymap;
        maxLuma = 255;
    }
    else if (prefix == "P6") {
        // NOTE: Portable PixMap (.ppm)
        pnmEncoding = PNMEncoding::Binary;
        pnmSubtype = PNMSubtype::Pixmap;
        maxLuma = 255;
    }
    else {
        return std::make_tuple(std::move(image), errors::make("The image is not PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }
    ++iter;

    skipCommentLine(iter, std::end(view));

    int width = 0;
    int height = 0;

    if (auto word = getWord(iter, std::end(view)); iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }
    else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), width); err != std::errc{}) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }
    ++iter;

    if (auto word = getWord(iter, std::end(view)); iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }
    else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), height); err != std::errc{}) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
    }
    ++iter;

    skipCommentLine(iter, std::end(view));

    switch (pnmSubtype) {
    case PNMSubtype::Bitmap:
        break;
    case PNMSubtype::Graymap:
        [[fallthrough]];
    case PNMSubtype::Pixmap:
        if (auto word = getWord(iter, std::end(view)); iter == std::end(view)) {
            return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
        }
        else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), maxLuma); err != std::errc{}) {
            return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
        }

        if ((iter != std::end(view)) && (*iter == '\n')) {
            ++iter;
        }
        else {
            return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
        }

        if ((maxLuma <= 0) || (maxLuma > 255)) {
            return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
        }
        break;
    }

    if (pnmEncoding != PNMEncoding::Binary) {
        skipCommentLine(iter, std::end(view));
    }

    if (width <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image width"));
    }
    if (height <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image height"));
    }
    if (width > std::numeric_limits<i16>::max()) {
        return std::make_tuple(std::move(image), errors::make("too large image width"));
    }
    if (height > std::numeric_limits<i16>::max()) {
        return std::make_tuple(std::move(image), errors::make("too large image height"));
    }

    switch (pnmSubtype) {
    case PNMSubtype::Bitmap:
        image.format = gpu::PixelFormat::R8_UNorm;
        break;
    case PNMSubtype::Graymap:
        image.format = gpu::PixelFormat::R8_UNorm;
        break;
    case PNMSubtype::Pixmap:
        image.format = gpu::PixelFormat::R8G8B8A8_UNorm;
        break;
    }

    image.width = width;
    image.height = height;

    const bool hasRGBChannels = (pnmSubtype == PNMSubtype::Pixmap);
    const std::size_t channelCount = hasRGBChannels ? 4 : 1;
    const auto maxComponentCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * channelCount;
    image.rawData.reserve(maxComponentCount);

    switch (pnmEncoding) {
    case PNMEncoding::ASCII:
        for (std::size_t i = 0; i < maxComponentCount; ++i) {
            if (hasRGBChannels && (i % 4 == 3)) {
                // NOTE: Insert alpha channel.
                image.rawData.push_back(255);
                continue;
            }

            // NOTE: Skip whitespaces
            const auto iterEnd = std::end(view);
            while ((iter != iterEnd) && ((*iter == ' ') || (*iter == '\r') || (*iter == '\n'))) {
                ++iter;
            }

            if (iter == iterEnd) {
                return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
            }

            int perChannel = 0;
            std::string_view word;
            if (pnmSubtype == PNMSubtype::Bitmap) {
                word = std::string_view{&(*iter), 1};
                ++iter;
            }
            else {
                word = getWord(iter, std::end(view));
            }

            if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), perChannel); err != std::errc{}) {
                return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
            }

            image.rawData.push_back(static_cast<u8>(perChannel));

            if (iter != std::end(view)) {
                if (*iter == '\n') {
                    skipCommentLine(iter, std::end(view));
                    ++iter;
                }
                else if ((*iter == ' ') || (*iter == '\r')) {
                    ++iter;
                }
            }
        }
        break;
    case PNMEncoding::Binary:
        switch (pnmSubtype) {
        case PNMSubtype::Bitmap: {
            const auto bytesPerRow = static_cast<std::size_t>((width + 7) / 8);
            for (int row = 0; row < height; ++row) {
                int pixelsInRow = 0;
                for (std::size_t byteIndex = 0; byteIndex < bytesPerRow; ++byteIndex) {
                    if (iter == std::end(view)) {
                        return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
                    }
                    const auto bits = *reinterpret_cast<const u8*>(&*iter);
                    ++iter;
                    for (int bit = 7; bit >= 0 && pixelsInRow < width; --bit) {
                        if (image.rawData.size() >= maxComponentCount) {
                            return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
                        }
                        image.rawData.push_back(static_cast<u8>((bits >> bit) & 1));
                        ++pixelsInRow;
                    }
                }
            }
            for (auto& b : image.rawData) {
                if (b != 0) {
                    b = 255;
                }
            }
            break;
        }
        case PNMSubtype::Graymap:
        case PNMSubtype::Pixmap:
            for (std::size_t i = 0; i < maxComponentCount; ++i) {
                if (hasRGBChannels && (i % 4 == 3)) {
                    // NOTE: Insert alpha channel.
                    image.rawData.push_back(255);
                    continue;
                }

                if (iter == std::end(view)) {
                    return std::make_tuple(std::move(image), errors::make("Invalid PNM format"));
                }

                image.rawData.push_back(*reinterpret_cast<const u8*>(&*iter));
                ++iter;
            }
            break;
        }
        break;
    }

    if (maxLuma == 1) {
        for (auto& c : image.rawData) {
            c = c * 255;
        }
    }
    else if (maxLuma != 255) {
        for (auto& c : image.rawData) {
            c = static_cast<u8>((static_cast<f32>(c) / maxLuma) * 255.0f);
        }
    }

    if (pnmSubtype == PNMSubtype::Bitmap) {
        for (auto& c : image.rawData) {
            c = 255 - c;
        }
    }

    image.pixelData = std::span<const u8>{image.rawData};

    return std::make_tuple(std::move(image), nullptr);
}

[[nodiscard]] std::tuple<std::vector<u8>, std::unique_ptr<Error>>
encodePNM(const Color* data, std::size_t size, int width, int height)
{
    PNMEncodeOptions options = {};
    options.encoding = PNMEncoding::Binary;
    options.subtype = PNMSubtype::Pixmap;
    options.maxValue = 255;
    return encodePNM(data, size, width, height, std::move(options));
}

[[nodiscard]] std::tuple<std::vector<u8>, std::unique_ptr<Error>>
encodePNM(const Color* data, std::size_t size, int width, int height, const PNMEncodeOptions& options)
{
    std::vector<u8> buffer = {};

    std::size_t channelCount = 1;
    std::size_t bytesPerComponent = 1;
    if (options.encoding == PNMEncoding::ASCII) {
        bytesPerComponent = 4;
    }
    if (options.subtype == PNMSubtype::Pixmap) {
        channelCount = 3;
    }
    std::size_t reserveSize = 3 + 6 + 6 + (bytesPerComponent * channelCount * static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    buffer.reserve(reserveSize);

    const auto writeString = [&buffer](std::string_view view) {
        if (view.empty()) {
            return;
        }
        const auto offset = buffer.size();
        buffer.resize(offset + view.size());
        std::memcpy(buffer.data() + offset, view.data(), view.size());
    };

    const auto writeByte = [&buffer](u8 v) {
        buffer.push_back(v);
    };

    switch (options.encoding) {
    case PNMEncoding::ASCII:
        switch (options.subtype) {
        case PNMSubtype::Bitmap:
            writeString("P1");
            break;
        case PNMSubtype::Graymap:
            writeString("P2");
            break;
        case PNMSubtype::Pixmap:
            writeString("P3");
            break;
        }
        break;
    case PNMEncoding::Binary:
        switch (options.subtype) {
        case PNMSubtype::Bitmap:
            writeString("P4");
            break;
        case PNMSubtype::Graymap:
            writeString("P5");
            break;
        case PNMSubtype::Pixmap:
            writeString("P6");
            break;
        }
        break;
    }
    writeString("\n");

    if (width <= 0) {
        return std::make_tuple(std::move(buffer), errors::make("width is too small"));
    }
    if (height <= 0) {
        return std::make_tuple(std::move(buffer), errors::make("height is too small"));
    }
    if (options.maxValue <= 0) {
        return std::make_tuple(std::move(buffer), errors::make("MaxValue is too small"));
    }

    writeString(std::to_string(width));
    writeString(" ");
    writeString(std::to_string(height));
    writeString("\n");

    if (options.subtype != PNMSubtype::Bitmap) {
        writeString(std::to_string(options.maxValue));
        writeString("\n");
    }

    const auto pixelCount = (width * height);

    if (size < static_cast<std::size_t>(pixelCount)) {
        return std::make_tuple(std::move(buffer), errors::make("size of pixel data is too small"));
    }

    switch (options.encoding) {
    case PNMEncoding::ASCII:
        switch (options.subtype) {
        case PNMSubtype::Bitmap:
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const auto& pixel = data[x + y * width];
                    if (pixel.g > 0) {
                        writeString("0");
                    }
                    else {
                        writeString("1");
                    }
                    if (x + 1 < width) {
                        writeString(" ");
                    }
                }
                writeString("\n");
            }
            break;
        case PNMSubtype::Graymap:
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const auto& pixel = data[x + y * width];
                    writeString(std::to_string(pixel.g));
                    if (x + 1 < width) {
                        writeString(" ");
                    }
                }
                writeString("\n");
            }
            break;
        case PNMSubtype::Pixmap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                writeString(std::to_string(pixel.r));
                writeString(" ");
                writeString(std::to_string(pixel.g));
                writeString(" ");
                writeString(std::to_string(pixel.b));
                writeString("\n");
            }
            break;
        }
        break;
    case PNMEncoding::Binary:
        switch (options.subtype) {
        case PNMSubtype::Bitmap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                if (pixel.g > 0) {
                    writeByte(0);
                }
                else {
                    writeByte(1);
                }
            }
            break;
        case PNMSubtype::Graymap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                writeByte(pixel.g);
            }
            break;
        case PNMSubtype::Pixmap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                writeByte(pixel.r);
                writeByte(pixel.g);
                writeByte(pixel.b);
            }
            break;
        }
        break;
    }

    return std::make_tuple(std::move(buffer), nullptr);
}

} // namespace pomdog
