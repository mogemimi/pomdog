// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT Image final {
public:
    Image(int width, int height);

    Image() = delete;
    Image(const Image&) = delete;
    Image(Image&&);

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&);

    int GetWidth() const noexcept;

    int GetHeight() const noexcept;

    const Color* GetData() const;

    void SetData(const Color* pixelData);

    void SetData(std::vector<Color>&& pixelData);

    const Color& GetPixel(int x, int y) const;

    void SetPixel(int x, int y, const Color& color);

    void Fill(const Color& color);

    void PremultiplyAlpha();

private:
    std::vector<Color> data;
    int width;
    int height;
};

} // namespace Pomdog
