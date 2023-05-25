// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image/image.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Image::Image(int widthIn, int heightIn)
    : width(widthIn)
    , height(heightIn)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    data.resize(width * height);
}

Image::Image(Image&& other)
{
    data = std::move(other.data);
    width = other.width;
    height = other.height;
}

Image& Image::operator=(Image&& other)
{
    data = std::move(other.data);
    width = other.width;
    height = other.height;
    return *this;
}

int Image::GetWidth() const noexcept
{
    return width;
}

int Image::GetHeight() const noexcept
{
    return height;
}

const Color* Image::GetData() const noexcept
{
    if (data.empty()) {
        return nullptr;
    }
    return data.data();
}

void Image::SetData(const Color* pixelData)
{
    POMDOG_ASSERT(pixelData != nullptr);
    std::memcpy(data.data(), pixelData, data.size() * sizeof(Color));
}

void Image::SetData(std::vector<Color>&& pixelData)
{
    POMDOG_ASSERT(!pixelData.empty());
    POMDOG_ASSERT(data.size() == pixelData.size());
    data = std::move(pixelData);
}

const Color& Image::GetPixel(int x, int y) const
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(x >= 0 && x < width);
    POMDOG_ASSERT(y >= 0 && y < height);
    POMDOG_ASSERT((width * height) == static_cast<int>(data.size()));

    const auto index = x + y * width;
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(data.size()));
    return data[index];
}

void Image::SetPixel(int x, int y, const Color& color)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(x >= 0 && x < width);
    POMDOG_ASSERT(y >= 0 && y < height);
    POMDOG_ASSERT((width * height) == static_cast<int>(data.size()));

    const auto index = x + y * width;
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(data.size()));
    data[index] = color;
}

void Image::Fill(const Color& color)
{
    std::fill(std::begin(data), std::end(data), color);
}

void Image::PremultiplyAlpha()
{
    for (auto& pixel : data) {
        const auto alpha = pixel.a;
        pixel = math::multiply(pixel, static_cast<float>(alpha) / 255.0f);
        pixel.a = alpha;
    }
}

} // namespace pomdog
