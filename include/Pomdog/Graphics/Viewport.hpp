// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_VIEWPORT_AA637108_HPP
#define POMDOG_VIEWPORT_AA637108_HPP

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Viewport {
public:
    int TopLeftX;
    int TopLeftY;
    int Width;
    int Height;
    float MinDepth;
    float MaxDepth;

public:
    Viewport() noexcept = default;

    explicit Viewport(Rectangle const& bounds) noexcept;

    Viewport(int x, int y, int width, int height) noexcept;

    Viewport(int x, int y, int width, int height,
        float minDepth, float maxDepth) noexcept;

    Vector3 Project(
        Vector3 const& source,
        Matrix4x4 const& worldViewProjection);

    Vector3 Unproject(
        Vector3 const& source,
        Matrix4x4 const& worldViewProjection);

    Rectangle GetBounds() const noexcept;

    float GetAspectRatio() const;
};

} // namespace Pomdog

#endif // POMDOG_VIEWPORT_AA637108_HPP
