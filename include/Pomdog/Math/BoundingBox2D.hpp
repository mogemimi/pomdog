// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <array>

namespace Pomdog {

class POMDOG_EXPORT BoundingBox2D {
public:
    Vector2 Min;
    Vector2 Max;

    static constexpr int CornerCount = 4;

public:
    BoundingBox2D() noexcept = default;

    BoundingBox2D(Vector2 const& min, Vector2 const& max);

    bool operator==(BoundingBox2D const&) const noexcept;
    bool operator!=(BoundingBox2D const&) const noexcept;

    ContainmentType Contains(Vector2 const& point) const;

    ContainmentType Contains(BoundingBox2D const& box) const;

    ContainmentType Contains(BoundingCircle const& circle) const;

    bool Intersects(BoundingBox2D const& box) const;

    bool Intersects(BoundingCircle const& circle) const;

    std::array<Vector2, CornerCount> GetCorners() const noexcept;
};

} // namespace Pomdog
