// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/cull_mode.hpp"
#include "pomdog/graphics/fill_mode.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT RasterizerDescription final {
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

    static RasterizerDescription CreateDefault() noexcept
    {
        return CreateCullCounterClockwise();
    }

    static RasterizerDescription CreateCullClockwise() noexcept
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::ClockwiseFace;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        return desc;
    }

    static RasterizerDescription CreateCullCounterClockwise() noexcept
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::CounterClockwiseFace;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        return desc;
    }

    static RasterizerDescription CreateCullNone() noexcept
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::None;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        return desc;
    }
};

} // namespace pomdog
