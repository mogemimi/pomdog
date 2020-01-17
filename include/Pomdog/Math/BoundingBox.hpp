// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <array>
#include <optional>

namespace Pomdog {

class POMDOG_EXPORT BoundingBox final {
public:
    Vector3 Min;
    Vector3 Max;

    static constexpr int CornerCount = 8;

public:
    BoundingBox() noexcept = default;

    BoundingBox(const Vector3& min, const Vector3& max);

    BoundingBox(const BoundingBox&) noexcept = default;
    BoundingBox(BoundingBox&&) noexcept = default;

    BoundingBox& operator=(const BoundingBox&) noexcept = default;
    BoundingBox& operator=(BoundingBox&&) noexcept = default;

    [[nodiscard]] bool operator==(const BoundingBox&) const noexcept;
    [[nodiscard]] bool operator!=(const BoundingBox&) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const Vector3& point) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingBox& box) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingSphere& sphere) const;

    [[nodiscard]] bool
    Intersects(const BoundingBox& box) const;

    [[nodiscard]] bool
    Intersects(const BoundingSphere& sphere) const;

    [[nodiscard]] PlaneIntersectionType
    Intersects(const Plane& plane) const;

    [[nodiscard]] std::optional<float>
    Intersects(const Ray& ray) const;

    [[nodiscard]] std::array<Vector3, CornerCount>
    GetCorners() const noexcept;
};

} // namespace Pomdog
