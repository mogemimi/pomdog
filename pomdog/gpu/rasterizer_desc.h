// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/fill_mode.h"

namespace pomdog::gpu {

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

    [[nodiscard]] static RasterizerDesc
    createDefault() noexcept;

    [[nodiscard]] static RasterizerDesc
    createCullClockwise() noexcept;

    [[nodiscard]] static RasterizerDesc
    createCullCounterClockwise() noexcept;

    [[nodiscard]] static RasterizerDesc
    createCullNone() noexcept;

    [[nodiscard]] static RasterizerDesc
    createCullNoneWireframe() noexcept;
};

} // namespace pomdog::gpu
