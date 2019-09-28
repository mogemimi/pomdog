// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"

namespace Pomdog {

class POMDOG_EXPORT BoundingCircle final {
public:
    Vector2 Center;
    float Radius;

public:
    BoundingCircle() noexcept = default;

    BoundingCircle(const Vector2& center, float radius);

    [[nodiscard]] bool operator==(const BoundingCircle&) const noexcept;
    [[nodiscard]] bool operator!=(const BoundingCircle&) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const Vector2& box) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingBox2D& box) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingCircle& circle) const;

    [[nodiscard]] bool
    Intersects(const BoundingBox2D& box) const;

    [[nodiscard]] bool
    Intersects(const BoundingCircle& circle) const;
};

} // namespace Pomdog
