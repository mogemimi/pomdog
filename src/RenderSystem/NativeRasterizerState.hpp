// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVERASTERIZERSTATE_F1119CE4_HPP
#define POMDOG_NATIVERASTERIZERSTATE_F1119CE4_HPP

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeGraphicsContext;

class NativeRasterizerState {
public:
    NativeRasterizerState() = default;
    NativeRasterizerState(NativeRasterizerState const&) = delete;
    NativeRasterizerState & operator=(NativeRasterizerState const&) = delete;

    virtual ~NativeRasterizerState() = default;

    virtual void Apply(NativeGraphicsContext & graphicsContext) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_NATIVERASTERIZERSTATE_F1119CE4_HPP
