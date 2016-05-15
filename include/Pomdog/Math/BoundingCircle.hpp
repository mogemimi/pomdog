// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT BoundingCircle {
public:
    Vector2 Center;
    float Radius;

public:
    BoundingCircle() noexcept = default;

    BoundingCircle(const Vector2& center, float radius);

    bool operator==(const BoundingCircle&) const noexcept;
    bool operator!=(const BoundingCircle&) const noexcept;

    ContainmentType Contains(const Vector2& box) const;

    ContainmentType Contains(const BoundingBox2D& box) const;

    ContainmentType Contains(const BoundingCircle& circle) const;

    bool Intersects(const BoundingBox2D& box) const;

    bool Intersects(const BoundingCircle& circle) const;
};

} // namespace Pomdog
