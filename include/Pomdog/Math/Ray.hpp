// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

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

    Ray(Vector3 const& position, Vector3 const& direction);

    bool operator==(Ray const&) const noexcept;
    bool operator!=(Ray const&) const noexcept;

    Optional<float> Intersects(BoundingBox const& box) const;

    Optional<float> Intersects(BoundingSphere const& sphere) const;
};

} // namespace Pomdog
