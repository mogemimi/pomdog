// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Math/BoundingBox2D.hpp"
#include "Pomdog/Math/BoundingCircle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

class Collider2D : public Component {
public:
    virtual ~Collider2D() = default;
};

class BoxCollider2D final : public Collider2D {
private:
    Vector2 size;

public:
    BoxCollider2D()
        : size(1.0f, 1.0f)
    {}

    BoundingBox2D GetCollider(const Vector2& position) const noexcept
    {
        const auto halfWidth = size.X / 2;
        const auto halfHeight = size.Y / 2;
        BoundingBox2D box;
        box.Min = position - Vector2{halfWidth, halfHeight};
        box.Max = position + Vector2{halfWidth, halfHeight};
        return box;
    }

    void SetWidth(float width) noexcept
    {
        POMDOG_ASSERT(width >= 0);
        size.X = width;
    }

    void SetHeight(float height) noexcept
    {
        POMDOG_ASSERT(height >= 0);
        size.Y = height;
    }
};

class CircleCollider2D final : public Collider2D {
private:
    float radius;

public:
    CircleCollider2D()
        : radius(1.0f)
    {}

    BoundingCircle GetCollider(const Vector2& position) const noexcept
    {
        BoundingCircle circle;
        circle.Radius = radius;
        circle.Center = position;
        return circle;
    }

    void SetRadius(float radiusIn) noexcept
    {
        POMDOG_ASSERT(radiusIn >= 0);
        radius = radiusIn;
    }
};

} // namespace Pomdog
