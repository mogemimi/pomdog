// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP
#define POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class SamplerStateDirect3D11 final: public NativeSamplerState {
public:
    SamplerStateDirect3D11() = delete;

    SamplerStateDirect3D11(ID3D11Device* nativeDevice, SamplerDescription const& description);

    ///@copydoc NativeSamplerState
    void Apply(NativeGraphicsContext & graphicsContext, int index) override;

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> nativeSamplerState;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP
