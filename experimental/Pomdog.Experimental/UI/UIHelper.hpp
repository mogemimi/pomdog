// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

namespace Pomdog {
namespace UI {

struct UIHelper {
    static Vector2 ConvertToChildSpace(
        Vector2 const& position, Matrix3x2 const& transform)
    {
        return Vector2::Transform(position, Matrix3x2::Invert(transform));
    }

    static Point2D ConvertToChildSpace(
        Point2D const& position, Matrix3x2 const& transform)
    {
        auto positionInChild = ConvertToChildSpace(Vector2(position.X, position.Y), transform);
        return Point2D(positionInChild.X, positionInChild.Y);
    }
};

} // namespace UI
} // namespace Pomdog
