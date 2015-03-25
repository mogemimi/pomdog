// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP
#define POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeGraphicsContext;

class NativeSamplerState {
public:
    NativeSamplerState() = default;
    NativeSamplerState(NativeSamplerState const&) = delete;
    NativeSamplerState & operator=(NativeSamplerState const&) = delete;

    virtual ~NativeSamplerState() = default;

    virtual void Apply(NativeGraphicsContext & graphicsContext, int index) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP)
