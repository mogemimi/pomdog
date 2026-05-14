// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image/image.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Image::Image(i32 widthIn, i32 heightIn)
    : width_(widthIn)
    , height_(heightIn)
{
    POMDOG_ASSERT(width_ > 0);
    POMDOG_ASSERT(height_ > 0);
    data_.resize(width_ * height_);
}

Image::Image(Image&& other)
{
    data_ = std::move(other.data_);
    width_ = other.width_;
    height_ = other.height_;
}

Image& Image::operator=(Image&& other)
{
    data_ = std::move(other.data_);
    width_ = other.width_;
    height_ = other.height_;
    return *this;
}

i32 Image::getWidth() const noexcept
{
    return width_;
}

i32 Image::getHeight() const noexcept
{
    return height_;
}

std::span<const Color>
Image::getData() const noexcept
{
    if (data_.empty()) {
        return {};
    }
    return data_;
}

void Image::setData(std::span<const Color> pixelData)
{
    POMDOG_ASSERT(!pixelData.empty());
    POMDOG_ASSERT(pixelData.data() != nullptr);
    POMDOG_ASSERT(data_.size() == pixelData.size());
    std::memcpy(data_.data(), pixelData.data(), sizeof(Color) * pixelData.size());
}

void Image::setData(std::vector<Color>&& pixelData)
{
    POMDOG_ASSERT(!pixelData.empty());
    POMDOG_ASSERT(data_.size() == pixelData.size());
    data_ = std::move(pixelData);
}

Color Image::getPixel(i32 x, i32 y) const
{
    POMDOG_ASSERT(width_ > 0);
    POMDOG_ASSERT(height_ > 0);
    POMDOG_ASSERT(x >= 0 && x < width_);
    POMDOG_ASSERT(y >= 0 && y < height_);
    POMDOG_ASSERT((width_ * height_) == static_cast<i32>(data_.size()));

    const auto index = x + y * width_;
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<i32>(data_.size()));
    return data_[index];
}

void Image::setPixel(i32 x, i32 y, const Color& color)
{
    POMDOG_ASSERT(width_ > 0);
    POMDOG_ASSERT(height_ > 0);
    POMDOG_ASSERT(x >= 0 && x < width_);
    POMDOG_ASSERT(y >= 0 && y < height_);
    POMDOG_ASSERT((width_ * height_) == static_cast<i32>(data_.size()));

    const auto index = x + y * width_;
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<i32>(data_.size()));
    data_[index] = color;
}

void Image::fill(const Color& color)
{
    std::fill(std::begin(data_), std::end(data_), color);
}

void Image::premultiplyAlpha()
{
    constexpr auto multiplyAlpha = [](u8 x, u8 alpha) -> u8 {
        return static_cast<u8>((static_cast<u32>(x) * static_cast<u32>(alpha) + 127) / 255);
    };

    for (auto& pixel : data_) {
        const auto alpha = pixel.a;
        if (alpha == 255) {
            continue;
        }
        if (alpha == 0) {
            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;
            continue;
        }
        pixel.r = multiplyAlpha(pixel.r, alpha);
        pixel.g = multiplyAlpha(pixel.g, alpha);
        pixel.b = multiplyAlpha(pixel.b, alpha);
    }
}

} // namespace pomdog
