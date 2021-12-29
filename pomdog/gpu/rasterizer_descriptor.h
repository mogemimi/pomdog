// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/fill_mode.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT RasterizerDescriptor final {
    /// A constant depth bias applied to a given pixel.
    ///
    /// The depth bias (or z-bias) can be added to each of the polygons that
    /// are coplanar in 3D space to make them appear as if they are not coplanar.
    std::int32_t DepthBias;

    /// A scale on a depth gradient of the primitive.
    float SlopeScaledDepthBias;

    /// Indicates whether to cull primitives for hidden surface removal.
    pomdog::CullMode CullMode;

    /// Determines the fill mode to use when rendering primitives.
    pomdog::FillMode FillMode;

    /// Enable multisample antialiasing (MSAA).
    bool MultisampleEnable;

    [[nodiscard]] static RasterizerDescriptor
    CreateDefault() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    CreateCullClockwise() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    CreateCullCounterClockwise() noexcept;

    [[nodiscard]] static RasterizerDescriptor
    CreateCullNone() noexcept;
};

} // namespace pomdog
