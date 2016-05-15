// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
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

    BoundingBox(const Vector3& min, const Vector3& max);

    bool operator==(const BoundingBox&) const noexcept;
    bool operator!=(const BoundingBox&) const noexcept;

    ContainmentType Contains(const Vector3& point) const;

    ContainmentType Contains(const BoundingBox& box) const;

    ContainmentType Contains(const BoundingSphere& sphere) const;

    bool Intersects(const BoundingBox& box) const;

    bool Intersects(const BoundingSphere& sphere) const;

    Optional<float> Intersects(const Ray& ray) const;

    std::array<Vector3, CornerCount> GetCorners() const noexcept;
};

} // namespace Pomdog
