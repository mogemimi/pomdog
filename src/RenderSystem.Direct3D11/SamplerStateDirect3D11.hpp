// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class SamplerStateDirect3D11 final : public NativeSamplerState {
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
