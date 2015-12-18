// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <array>

namespace Pomdog {

class POMDOG_EXPORT BoundingBox {
public:
    Vector3 Min;
    Vector3 Max;

    static constexpr int CornerCount = 8;

public:
    BoundingBox() noexcept = default;

    BoundingBox(Vector3 const& min, Vector3 const& max);

    bool operator==(BoundingBox const&) const noexcept;
    bool operator!=(BoundingBox const&) const noexcept;

    ContainmentType Contains(Vector3 const& point) const;

    ContainmentType Contains(BoundingBox const& box) const;

    ContainmentType Contains(BoundingSphere const& sphere) const;

    bool Intersects(BoundingBox const& box) const;

    bool Intersects(BoundingSphere const& sphere) const;

    Optional<float> Intersects(Ray const& ray) const;

    std::array<Vector3, CornerCount> GetCorners() const noexcept;
};

} // namespace Pomdog
