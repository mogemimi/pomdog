// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP
#define POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class SamplerStateDirect3D11 final: public NativeSamplerState {
public:
    SamplerStateDirect3D11(
        ID3D11Device* device,
        SamplerDescription const& description);

    ID3D11SamplerState* GetSamplerState() const;

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_HPP
