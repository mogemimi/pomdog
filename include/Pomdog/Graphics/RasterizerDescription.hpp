// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/CullMode.hpp"
#include "Pomdog/Graphics/FillMode.hpp"
#include <cstdint>

namespace Pomdog {

struct POMDOG_EXPORT RasterizerDescription final {
    std::int32_t DepthBias;
    float SlopeScaledDepthBias;
    Pomdog::CullMode CullMode;
    Pomdog::FillMode FillMode;
    bool MultisampleEnable;
    bool ScissorTestEnable;

    static RasterizerDescription CreateDefault()
    {
        return CreateCullCounterClockwise();
    }

    static RasterizerDescription CreateCullClockwise()
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::ClockwiseFace;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        desc.ScissorTestEnable = false;
        return desc;
    }

    static RasterizerDescription CreateCullCounterClockwise()
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::CounterClockwiseFace;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        desc.ScissorTestEnable = false;
        return desc;
    }

    static RasterizerDescription CreateCullNone()
    {
        RasterizerDescription desc;
        desc.CullMode = CullMode::None;
        desc.FillMode = FillMode::Solid;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.MultisampleEnable = false;
        desc.ScissorTestEnable = false;
        return desc;
    }
};

} // namespace Pomdog
