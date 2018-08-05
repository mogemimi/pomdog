// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>
#include <limits>

namespace Pomdog {

Viewport::Viewport(const Rectangle& bounds) noexcept
    : TopLeftX(bounds.X)
    , TopLeftY(bounds.Y)
    , Width(bounds.Width)
    , Height(bounds.Height)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{
}

Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn) noexcept
    : TopLeftX(xIn)
    , TopLeftY(yIn)
    , Width(widthIn)
    , Height(heightIn)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{
}

Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn,
    float minDepthIn, float maxDepthIn) noexcept
    : TopLeftX(xIn)
    , TopLeftY(yIn)
    , Width(widthIn)
    , Height(heightIn)
    , MinDepth(minDepthIn)
    , MaxDepth(maxDepthIn)
{
}

Vector3 Viewport::Project(
    const Vector3& source,
    const Matrix4x4& worldViewProjection) const
{
    auto result = Vector3::Transform(source, worldViewProjection);
    auto divisor = source.X * worldViewProjection(0, 3)
        + source.Y * worldViewProjection(1, 3)
        + source.Z * worldViewProjection(2, 3)
        + worldViewProjection(3, 3);

    if (std::abs(divisor) > std::numeric_limits<float>::epsilon()) {
        result = result / divisor;
    }

    result.X = (result.X + 1.0f) * 0.5f * static_cast<float>(Width) + static_cast<float>(TopLeftX);
    result.Y = (-result.Y + 1.0f) * 0.5f * static_cast<float>(Height) + static_cast<float>(TopLeftY);
    result.Z = result.Z * (MaxDepth - MinDepth) + MinDepth;
    return result;
}

Vector3 Viewport::Unproject(
    const Vector3& source,
    const Matrix4x4& worldViewProjection) const
{
    POMDOG_ASSERT(Width > 0);
    POMDOG_ASSERT(Height > 0);
    POMDOG_ASSERT((MaxDepth - MinDepth) != 0.0f);

    Vector3 vec;
    vec.X = ((source.X - static_cast<float>(TopLeftX)) / static_cast<float>(Width) * 2.0f) - 1.0f;
    vec.Y = -(((source.Y - static_cast<float>(TopLeftY)) / static_cast<float>(Height) * 2.0f) - 1.0f);
    vec.Z = (source.Z - MinDepth) / (MaxDepth - MinDepth);

    Matrix4x4 invertWVP = Matrix4x4::Invert(worldViewProjection);
    auto result = Vector3::Transform(vec, invertWVP);
    auto divisor = vec.X * invertWVP(0, 3)
        + vec.Y * invertWVP(1, 3)
        + vec.Z * invertWVP(2, 3)
        + invertWVP(3, 3);

    if (std::abs(divisor) > std::numeric_limits<float>::epsilon()) {
        result = result / divisor;
    }
    return result;
}

Rectangle Viewport::GetBounds() const noexcept
{
    return Rectangle{TopLeftX, TopLeftY, Width, Height};
}

float Viewport::GetAspectRatio() const
{
    POMDOG_ASSERT(this->Height != 0);
    if (this->Height != 0) {
        return static_cast<float>(this->Width) / this->Height;
    }
    return 0.0f;
}

} // namespace Pomdog
