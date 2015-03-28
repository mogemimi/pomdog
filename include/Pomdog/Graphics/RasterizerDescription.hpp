// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RASTERIZERDESCRIPTION_AD753EE4_HPP
#define POMDOG_RASTERIZERDESCRIPTION_AD753EE4_HPP

#include "CullMode.hpp"
#include "FillMode.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT RasterizerDescription {
public:
    std::int32_t DepthBias = 0;

    float SlopeScaledDepthBias = 0;

    CullMode CullMode = CullMode::CounterClockwiseFace;

    FillMode FillMode = FillMode::Solid;

    bool MultisampleEnable = false;

    bool ScissorTestEnable = false;

public:
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

}// namespace Pomdog

#endif // POMDOG_RASTERIZERDESCRIPTION_AD753EE4_HPP
