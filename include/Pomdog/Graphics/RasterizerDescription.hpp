// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RASTERIZERDESCRIPTION_AD753EE4_3D3A_44B2_85B3_A54B2C5D1B4A_HPP
#define POMDOG_RASTERIZERDESCRIPTION_AD753EE4_3D3A_44B2_85B3_A54B2C5D1B4A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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
};

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERDESCRIPTION_AD753EE4_3D3A_44B2_85B3_A54B2C5D1B4A_HPP)
