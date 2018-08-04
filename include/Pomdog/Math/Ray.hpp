// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Optional.hpp"

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
