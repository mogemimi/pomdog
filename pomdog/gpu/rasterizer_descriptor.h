// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/fill_mode.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT RasterizerDescriptor final {
    /// A constant depth bias applied to a given pixel.
    ///
    /// The depth bias (or z-bias) can be added to each of the polygons that
    /// are coplanar in 3D space to make them appear as if they are not coplanar.
    std::int32_t depthBias;

    /// A scale on a depth gradient of the primitive.
    float slopeScaledDepthBias;

    /// Indicates whether to cull primitives for hidden surface removal.
    CullMode cullMode;

    /// Determines the fill mode to use when rendering primitives.
    FillMode fillMode;

    /// Enable multisample antialiasing (MSAA).
    bool multisampleEnable;

    [[nodiscard]] static RasterizerDescriptor
    createDefault() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    createCullClockwise() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    createCullCounterClockwise() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    createCullNone() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    createCullNoneWireframe() noexcept;
};

} // namespace pomdog::gpu
