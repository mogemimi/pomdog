// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVESAMPLERSTATE_4A6E443C_HPP
#define POMDOG_NATIVESAMPLERSTATE_4A6E443C_HPP

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
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_NATIVESAMPLERSTATE_4A6E443C_HPP
