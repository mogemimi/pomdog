// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"

namespace Pomdog {

class POMDOG_EXPORT Viewport final {
public:
    int TopLeftX;
    int TopLeftY;
    int Width;
    int Height;

    // The minimum depth of the viewport. It must be between 0.0 and 1.0.
    float MinDepth;

    // The maximum depth of the viewport. It must be between 0.0 and 1.0.
    float MaxDepth;

public:
    Viewport() noexcept = default;

    explicit Viewport(const Rectangle& bounds) noexcept;

    Viewport(int x, int y, int width, int height) noexcept;

    Viewport(int x, int y, int width, int height,
        float minDepth, float maxDepth) noexcept;

    Vector3 Project(
        const Vector3& source,
        const Matrix4x4& worldViewProjection) const;

    Vector3 Unproject(
        const Vector3& source,
        const Matrix4x4& worldViewProjection) const;

    Rectangle GetBounds() const noexcept;

    float GetAspectRatio() const;
};

} // namespace Pomdog
