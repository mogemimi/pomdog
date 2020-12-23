// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/CullMode.hpp"
#include "Pomdog/Graphics/FillMode.hpp"
#include <cstdint>

namespace Pomdog {

struct POMDOG_EXPORT RasterizerDescription final {
    /// A constant depth bias applied to a given pixel.
    ///
    /// The depth bias (or z-bias) can be added to each of the polygons that
    /// are coplanar in 3D space to make them appear as if they are not coplanar.
    std::int32_t DepthBias;

    /// A scale on a depth gradient of the primitive.
    float SlopeScaledDepthBias;

    /// Indicates whether to cull primitives for hidden surface removal.
    Pomdog::CullMode CullMode;

    /// Determines the fill mode to use when rendering primitives.
    Pomdog::FillMode FillMode;

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

} // namespace Pomdog
