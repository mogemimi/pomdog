// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP
#define POMDOG_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

#endif // !defined(POMDOG_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP)
