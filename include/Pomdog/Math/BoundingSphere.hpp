// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BOUNDINGSPHERE_4FA29CAA_HPP
#define POMDOG_BOUNDINGSPHERE_4FA29CAA_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT BoundingSphere {
public:
    Vector3 Center;
    float Radius;

public:
    BoundingSphere() noexcept = default;

    BoundingSphere(Vector3 const& center, float radius);

    bool operator==(BoundingSphere const&) const;
    bool operator!=(BoundingSphere const&) const;

    ContainmentType Contains(Vector3 const& point) const;

    ContainmentType Contains(BoundingBox const& box) const;

    ContainmentType Contains(BoundingSphere const& sphere) const;

    bool Intersects(BoundingBox const& box) const;

    bool Intersects(BoundingSphere const& sphere) const;

    Optional<float> Intersects(Ray const& ray) const;
};

} // namespace Pomdog

#endif // POMDOG_BOUNDINGSPHERE_4FA29CAA_HPP
