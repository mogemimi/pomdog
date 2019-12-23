// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <array>

namespace Pomdog {

class POMDOG_EXPORT BoundingBox2D final {
public:
    Vector2 Min;
    Vector2 Max;

    static constexpr int CornerCount = 4;

public:
    BoundingBox2D() noexcept = default;

    BoundingBox2D(const Vector2& min, const Vector2& max);

    BoundingBox2D(const BoundingBox2D&) noexcept = default;
    BoundingBox2D(BoundingBox2D&&) noexcept = default;

    BoundingBox2D& operator=(const BoundingBox2D&) noexcept = default;
    BoundingBox2D& operator=(BoundingBox2D&&) noexcept = default;

    [[nodiscard]] bool operator==(const BoundingBox2D&) const noexcept;
    [[nodiscard]] bool operator!=(const BoundingBox2D&) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const Vector2& point) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingBox2D& box) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingCircle& circle) const;

    [[nodiscard]] bool
    Intersects(const BoundingBox2D& box) const;

    [[nodiscard]] bool
    Intersects(const BoundingCircle& circle) const;

    [[nodiscard]] std::array<Vector2, CornerCount>
    GetCorners() const noexcept;
};

} // namespace Pomdog
