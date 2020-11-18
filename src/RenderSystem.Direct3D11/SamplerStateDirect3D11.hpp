// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class SamplerStateDirect3D11 final : public SamplerState {
public:
    SamplerStateDirect3D11(
        ID3D11Device* device,
        const SamplerDescription& description);

    ID3D11SamplerState* GetSamplerState() const;

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

} // namespace Pomdog::Detail::Direct3D11
