// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Ray {
public:
    Vector3 Position;
    Vector3 Direction;

public:
    Ray() = default;

    Ray(const Vector3& position, const Vector3& direction);

    bool operator==(const Ray&) const noexcept;
    bool operator!=(const Ray&) const noexcept;

    Optional<float> Intersects(const BoundingBox& box) const;

    Optional<float> Intersects(const BoundingFrustum& frustum) const;

    Optional<float> Intersects(const BoundingSphere& sphere) const;

    Optional<float> Intersects(const Plane& plane) const;
};

} // namespace Pomdog
