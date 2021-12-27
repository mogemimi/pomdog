// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/image/pnm.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/color.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <charconv>
#include <cstring>
#include <string_view>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::PNM {
namespace {

template <class StringIterator>
std::string_view
GetWord(StringIterator& it, StringIterator end)
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
void SkipCommentLine(StringIterator& it, StringIterator end)
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

[[nodiscard]] std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(const char* data, std::size_t size)
{
    ImageBuffer image;
    image.PixelData = nullptr;
    image.ByteLength = 0;
    image.MipmapCount = 0;

    if (size < 7) {
        return std::make_tuple(std::move(image), errors::New("The PNM data size is too small"));
    }

    const std::string_view view{data, size};

    PNMEncoding pnmEncoding = PNMEncoding::ASCII;
    PNMSubtype pnmSubtype = PNMSubtype::Bitmap;
    int maxLuma = 1;

    auto iter = std::begin(view);

    SkipCommentLine(iter, std::end(view));

    if (auto prefix = GetWord(iter, std::end(view)); prefix == "P1") {
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
        return std::make_tuple(std::move(image), errors::New("The image is not PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }
    ++iter;

    SkipCommentLine(iter, std::end(view));

    int width = 0;
    int height = 0;

    if (auto word = GetWord(iter, std::end(view)); iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }
    else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), width); err != std::errc{}) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }
    ++iter;

    if (auto word = GetWord(iter, std::end(view)); iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }
    else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), height); err != std::errc{}) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }

    if (iter == std::end(view)) {
        return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
    }
    ++iter;

    SkipCommentLine(iter, std::end(view));

    switch (pnmSubtype) {
    case PNMSubtype::Bitmap:
        break;
    case PNMSubtype::Graymap:
        [[fallthrough]];
    case PNMSubtype::Pixmap:
        if (auto word = GetWord(iter, std::end(view)); iter == std::end(view)) {
            return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
        }
        else if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), maxLuma); err != std::errc{}) {
            return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
        }

        if ((iter != std::end(view)) && (*iter == '\n')) {
            ++iter;
        }
        else {
            return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
        }

        if ((maxLuma <= 0) || (maxLuma > 255)) {
            return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
        }
        break;
    }

    if (pnmEncoding != PNMEncoding::Binary) {
        SkipCommentLine(iter, std::end(view));
    }

    if ((width <= 0) || (height <= 0)) {
        return std::make_tuple(std::move(image), errors::New("The PNM image is too small"));
    }
    if ((width > 32767) || (height > 32767)) {
        return std::make_tuple(std::move(image), errors::New("The PNM image is too large"));
    }

    switch (pnmSubtype) {
    case PNMSubtype::Bitmap:
        image.Format = SurfaceFormat::R8_UNorm;
        break;
    case PNMSubtype::Graymap:
        image.Format = SurfaceFormat::R8_UNorm;
        break;
    case PNMSubtype::Pixmap:
        image.Format = SurfaceFormat::R8G8B8A8_UNorm;
        break;
    }

    image.Width = width;
    image.Height = height;

    const bool hasRGBChannels = (pnmSubtype == PNMSubtype::Pixmap);
    const std::size_t channelCount = hasRGBChannels ? 4 : 1;
    const auto maxComponentCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * channelCount;
    image.RawData.reserve(maxComponentCount);

    switch (pnmEncoding) {
    case PNMEncoding::ASCII:
        for (std::size_t i = 0; i < maxComponentCount; ++i) {
            if (hasRGBChannels && (i % 4 == 3)) {
                // NOTE: Insert alpha channel.
                image.RawData.push_back(255);
                continue;
            }

            // NOTE: Skip whitespaces
            const auto iterEnd = std::end(view);
            while ((iter != iterEnd) && ((*iter == ' ') || (*iter == '\r') || (*iter == '\n'))) {
                ++iter;
            }

            if (iter == iterEnd) {
                return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
            }

            int perChannel = 0;
            std::string_view word;
            if (pnmSubtype == PNMSubtype::Bitmap) {
                word = std::string_view{&(*iter), 1};
                ++iter;
            }
            else {
                word = GetWord(iter, std::end(view));
            }

            if (auto [p, err] = std::from_chars(word.data(), word.data() + word.size(), perChannel); err != std::errc{}) {
                return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
            }

            image.RawData.push_back(static_cast<std::uint8_t>(perChannel));

            if (iter != std::end(view)) {
                if (*iter == '\n') {
                    SkipCommentLine(iter, std::end(view));
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
        case PNMSubtype::Bitmap:
            while (iter != std::end(view)) {
                if (image.RawData.capacity() <= image.RawData.size()) {
                    return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
                }
                const auto bits = *reinterpret_cast<const std::uint8_t*>(&*iter);
                image.RawData.push_back(bits & 0b10000000);
                image.RawData.push_back(bits & 0b01000000);
                image.RawData.push_back(bits & 0b00100000);
                image.RawData.push_back(bits & 0b00010000);
                image.RawData.push_back(bits & 0b00001000);
                image.RawData.push_back(bits & 0b00000100);
                image.RawData.push_back(bits & 0b00000010);
                image.RawData.push_back(bits & 0b00000001);
                ++iter;
            }
            for (auto& b : image.RawData) {
                if (b != 0) {
                    b = 255;
                }
            }
            break;
        case PNMSubtype::Graymap:
        case PNMSubtype::Pixmap:
            for (std::size_t i = 0; i < maxComponentCount; ++i) {
                if (hasRGBChannels && (i % 4 == 3)) {
                    // NOTE: Insert alpha channel.
                    image.RawData.push_back(255);
                    continue;
                }

                if (iter == std::end(view)) {
                    return std::make_tuple(std::move(image), errors::New("Invalid PNM format"));
                }

                image.RawData.push_back(*reinterpret_cast<const std::uint8_t*>(&*iter));
                ++iter;
            }
            break;
        }
        break;
    }

    if (maxLuma == 1) {
        for (auto& c : image.RawData) {
            c = c * 255;
        }
    }
    else if (maxLuma != 255) {
        for (auto& c : image.RawData) {
            c = static_cast<std::uint8_t>((static_cast<float>(c) / maxLuma) * 255.0f);
        }
    }

    if (pnmSubtype == PNMSubtype::Bitmap) {
        for (auto& c : image.RawData) {
            c = 255 - c;
        }
    }

    image.PixelData = image.RawData.data();
    image.ByteLength = image.RawData.size();

    return std::make_tuple(std::move(image), nullptr);
}

std::tuple<std::vector<std::uint8_t>, std::unique_ptr<Error>>
Encode(const Color* data, std::size_t size, int width, int height)
{
    PNMEncodeOptions options;
    options.Encoding = PNMEncoding::Binary;
    options.Subtype = PNMSubtype::Pixmap;
    options.MaxValue = 255;
    return Encode(data, size, width, height, std::move(options));
}

std::tuple<std::vector<std::uint8_t>, std::unique_ptr<Error>>
Encode(const Color* data, std::size_t size, int width, int height, const PNMEncodeOptions& options)
{
    std::vector<std::uint8_t> buffer;

    std::size_t channelCount = 1;
    std::size_t bytesPerComponent = 1;
    if (options.Encoding == PNMEncoding::ASCII) {
        bytesPerComponent = 4;
    }
    if (options.Subtype == PNMSubtype::Pixmap) {
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

    const auto writeByte = [&buffer](std::uint8_t v) {
        buffer.push_back(v);
    };

    switch (options.Encoding) {
    case PNMEncoding::ASCII:
        switch (options.Subtype) {
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
        switch (options.Subtype) {
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
        return std::make_tuple(std::move(buffer), errors::New("width is too small"));
    }
    if (height <= 0) {
        return std::make_tuple(std::move(buffer), errors::New("height is too small"));
    }
    if (options.MaxValue <= 0) {
        return std::make_tuple(std::move(buffer), errors::New("MaxValue is too small"));
    }

    writeString(std::to_string(width));
    writeString(" ");
    writeString(std::to_string(height));
    writeString("\n");

    if (options.Subtype != PNMSubtype::Bitmap) {
        writeString(std::to_string(options.MaxValue));
        writeString("\n");
    }

    const auto pixelCount = (width * height);

    if (size < static_cast<std::size_t>(pixelCount)) {
        return std::make_tuple(std::move(buffer), errors::New("size of pixel data is too small"));
    }

    switch (options.Encoding) {
    case PNMEncoding::ASCII:
        switch (options.Subtype) {
        case PNMSubtype::Bitmap:
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const auto& pixel = data[x + y * width];
                    if (pixel.G > 0) {
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
                    writeString(std::to_string(pixel.G));
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
                writeString(std::to_string(pixel.R));
                writeString(" ");
                writeString(std::to_string(pixel.G));
                writeString(" ");
                writeString(std::to_string(pixel.B));
                writeString("\n");
            }
            break;
        }
        break;
    case PNMEncoding::Binary:
        switch (options.Subtype) {
        case PNMSubtype::Bitmap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                if (pixel.G > 0) {
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
                writeByte(pixel.G);
            }
            break;
        case PNMSubtype::Pixmap:
            for (int i = 0; i < pixelCount; ++i) {
                const auto& pixel = data[i];
                writeByte(pixel.R);
                writeByte(pixel.G);
                writeByte(pixel.B);
            }
            break;
        }
        break;
    }

    return std::make_tuple(std::move(buffer), nullptr);
}

} // namespace pomdog::PNM
