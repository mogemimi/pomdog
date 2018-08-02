// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Image/Image.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstring>
#include <utility>

namespace Pomdog {

Image::Image(int widthIn, int heightIn)
    : width(widthIn)
    , height(heightIn)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    data.resize(width * height);
}

Image::Image(Image && other)
{
    data = std::move(other.data);
    width = other.width;
    height = other.height;
}

Image & Image::operator=(Image && other)
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

const Color* Image::GetData() const
{
    return data.data();
}

void Image::SetData(const Color* pixelData)
{
    POMDOG_ASSERT(pixelData != nullptr);
    std::memcpy(data.data(), pixelData, data.size());
}

void Image::SetData(std::vector<Color> && pixelData)
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

void Image::Fill(const Color &color)
{
    std::fill(std::begin(data), std::end(data), color);
}

void Image::PremultiplyAlpha()
{
    for (auto & pixel : data) {
        const auto alpha = pixel.A;
        pixel = Color::Multiply(pixel, static_cast<float>(alpha) / 255.0f);
        pixel.A = alpha;
    }
}

} // namespace Pomdog
