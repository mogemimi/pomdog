// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

    bool operator==(const BoundingBox2D&) const noexcept;
    bool operator!=(const BoundingBox2D&) const noexcept;

    ContainmentType Contains(const Vector2& point) const;

    ContainmentType Contains(const BoundingBox2D& box) const;

    ContainmentType Contains(const BoundingCircle& circle) const;

    bool Intersects(const BoundingBox2D& box) const;

    bool Intersects(const BoundingCircle& circle) const;

    std::array<Vector2, CornerCount> GetCorners() const noexcept;
};

} // namespace Pomdog
