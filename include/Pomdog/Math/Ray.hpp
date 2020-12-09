// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <optional>

namespace Pomdog {

class POMDOG_EXPORT Ray final {
public:
    Vector3 Position;
    Vector3 Direction;

public:
    Ray() = default;

    Ray(const Vector3& position, const Vector3& direction);

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingBox& box) const;

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingFrustum& frustum) const;

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingSphere& sphere) const;

    [[nodiscard]] std::optional<float>
    Intersects(const Plane& plane) const;
};

} // namespace Pomdog
