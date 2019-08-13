// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class SamplerStateDirect3D11 final : public NativeSamplerState {
public:
    SamplerStateDirect3D11(
        ID3D11Device* device,
        const SamplerDescription& description);

    ID3D11SamplerState* GetSamplerState() const;

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

} // namespace Pomdog::Detail::Direct3D11
