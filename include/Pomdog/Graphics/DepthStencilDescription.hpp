// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP
#define POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP

#include "ComparisonFunction.hpp"
#include "DepthStencilOperation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>

namespace Pomdog {

class POMDOG_EXPORT DepthStencilDescription {
public:
    DepthStencilOperation ClockwiseFace;

    DepthStencilOperation CounterClockwiseFace;

    std::int32_t ReferenceStencil = 0;

    std::uint32_t StencilMask = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t StencilWriteMask = std::numeric_limits<std::uint32_t>::max();

    ComparisonFunction DepthBufferFunction = ComparisonFunction::LessEqual;

    bool DepthBufferEnable = true;

    bool DepthBufferWriteEnable = true;

    bool StencilEnable = false;
};

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP)
