// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT Image final {
public:
    Image(int width, int height);

    Image() = delete;
    Image(const Image&) = delete;
    Image(Image&&);

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&);

    /// Gets the width of the image data, in pixels.
    [[nodiscard]] int GetWidth() const noexcept;

    /// Gets the height of the image data, in pixels.
    [[nodiscard]] int GetHeight() const noexcept;

    /// Gets image data.
    const Color* GetData() const noexcept;

    /// Sets image data.
    void SetData(const Color* pixelData);

    /// Sets image data.
    void SetData(std::vector<Color>&& pixelData);

    [[nodiscard]] const Color& GetPixel(int x, int y) const;

    void SetPixel(int x, int y, const Color& color);

    void Fill(const Color& color);

    void PremultiplyAlpha();

private:
    std::vector<Color> data;
    int width;
    int height;
};

} // namespace pomdog
