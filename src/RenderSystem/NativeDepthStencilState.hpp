// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_HPP
#define POMDOG_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_HPP

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeGraphicsContext;

class NativeDepthStencilState {
public:
    NativeDepthStencilState() = default;
    NativeDepthStencilState(NativeDepthStencilState const&) = delete;
    NativeDepthStencilState & operator=(NativeDepthStencilState const&) = delete;

    virtual ~NativeDepthStencilState() = default;

    virtual void Apply(NativeGraphicsContext & graphicsContext) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_HPP
