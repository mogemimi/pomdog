// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VIEWPORT_AA637108_HPP
#define POMDOG_VIEWPORT_AA637108_HPP

#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT Viewport {
public:
    Rectangle Bounds;
    float MinDepth;
    float MaxDepth;

public:
    Viewport() = default;

    explicit Viewport(Rectangle const& bounds);

    Viewport(std::int32_t x, std::int32_t y,
        std::int32_t width, std::int32_t height);

    Viewport(std::int32_t x, std::int32_t y,
        std::int32_t width, std::int32_t height,
        float minDepth, float maxDepth);

    void Width(std::int32_t width) noexcept;

    std::int32_t Width() const noexcept;

    void Height(std::int32_t height) noexcept;

    std::int32_t Height() const noexcept;

    std::int32_t TopLeftX() const noexcept;

    std::int32_t TopLeftY() const noexcept;

    float AspectRatio() const;
};

} // namespace Pomdog

#endif // POMDOG_VIEWPORT_AA637108_HPP
