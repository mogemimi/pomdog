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
    Viewport() noexcept = default;

    explicit Viewport(Rectangle const& bounds) noexcept;

    Viewport(std::int32_t x, std::int32_t y,
        std::int32_t width, std::int32_t height) noexcept;

    Viewport(std::int32_t x, std::int32_t y,
        std::int32_t width, std::int32_t height,
        float minDepth, float maxDepth) noexcept;

    std::int32_t GetX() const noexcept;

    void SetX(std::int32_t x) noexcept;

    std::int32_t GetY() const noexcept;

    void SetY(std::int32_t y) noexcept;

    std::int32_t GetWidth() const noexcept;

    void SetWidth(std::int32_t width) noexcept;

    std::int32_t GetHeight() const noexcept;

    void SetHeight(std::int32_t height) noexcept;

    float GetAspectRatio() const;
};

} // namespace Pomdog

#endif // POMDOG_VIEWPORT_AA637108_HPP
