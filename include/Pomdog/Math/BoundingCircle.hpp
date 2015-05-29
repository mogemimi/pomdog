// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BOUNDINGCIRCLE_00F66014_HPP
#define POMDOG_BOUNDINGCIRCLE_00F66014_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT BoundingCircle {
public:
	Vector2 Center;
	float Radius;

public:
    BoundingCircle() noexcept = default;

    BoundingCircle(Vector2 const& center, float radius);

    bool operator==(BoundingCircle const&) const;
    bool operator!=(BoundingCircle const&) const;

    ContainmentType Contains(Vector2 const& box) const;

    ContainmentType Contains(BoundingBox2D const& box) const;

    ContainmentType Contains(BoundingCircle const& circle) const;

    bool Intersects(BoundingBox2D const& box) const;

    bool Intersects(BoundingCircle const& circle) const;
};

} // namespace Pomdog

#endif // POMDOG_BOUNDINGCIRCLE_00F66014_HPP
