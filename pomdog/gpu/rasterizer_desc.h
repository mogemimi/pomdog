// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/fill_mode.h"

namespace pomdog::gpu {

/// RasterizerDesc describes the rasterizer stage state for a graphics pipeline.
struct POMDOG_EXPORT RasterizerDesc final {
    /// A constant depth bias applied to a given pixel.
    ///
    /// The depth bias (or z-bias) can be added to each of the polygons that
    /// are coplanar in 3D space to make them appear as if they are not coplanar.
    i32 depthBias = 0;

    /// A scale on a depth gradient of the primitive.
    f32 slopeScaledDepthBias = 0;

    /// Indicates whether to cull primitives for hidden surface removal.
    CullMode cullMode = CullMode::CounterClockwiseFace;

    /// Determines the fill mode to use when rendering primitives.
    FillMode fillMode = FillMode::Solid;

    /// Enable multisample antialiasing (MSAA).
    bool multisampleEnable = false;

    /// Returns a rasterizer state equivalent to `createCullCounterClockwise()`.
    [[nodiscard]] static RasterizerDesc
    createDefault() noexcept;

    /// Returns a solid-fill rasterizer state that culls clockwise-wound faces.
    [[nodiscard]] static RasterizerDesc
    createCullClockwise() noexcept;

    /// Returns a solid-fill rasterizer state that culls counter-clockwise-wound faces.
    [[nodiscard]] static RasterizerDesc
    createCullCounterClockwise() noexcept;

    /// Returns a solid-fill rasterizer state with no face culling.
    [[nodiscard]] static RasterizerDesc
    createCullNone() noexcept;

    /// Returns a wireframe rasterizer state with no face culling.
    [[nodiscard]] static RasterizerDesc
    createCullNoneWireframe() noexcept;
};

} // namespace pomdog::gpu
