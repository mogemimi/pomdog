// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_HPP
#define POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class DepthStencilStateDirect3D11 final: public NativeDepthStencilState {
public:
    DepthStencilStateDirect3D11() = delete;

    DepthStencilStateDirect3D11(ID3D11Device* nativeDevice, DepthStencilDescription const& description);

    ///@copydoc NativeDepthStencilState
    void Apply(NativeGraphicsContext & graphicsContext) override;

private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> nativeDepthStencilState;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_HPP
