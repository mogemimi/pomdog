// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDSTATEDIRECT3D11_24607C21_74E1_49B1_B7AB_033D7F5B8E4B_HPP
#define POMDOG_BLENDSTATEDIRECT3D11_24607C21_74E1_49B1_B7AB_033D7F5B8E4B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeBlendState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector4.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class BlendStateDirect3D11 final: public NativeBlendState {
public:
    BlendStateDirect3D11() = delete;

    BlendStateDirect3D11(ID3D11Device* nativeDevice, BlendDescription const& description);

    ///@copydoc NativeBlendState
    void Apply(NativeGraphicsContext & graphicsContext) override;

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> nativeBlendState;
    Vector4 const blendFactor;
    UINT const multiSampleMask;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_BLENDSTATEDIRECT3D11_24607C21_74E1_49B1_B7AB_033D7F5B8E4B_HPP)
