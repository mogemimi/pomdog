// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <d3d12.h>
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class SamplerStateDirect3D12 final : public NativeSamplerState {
public:
    SamplerStateDirect3D12(
        ID3D12Device* device,
        SamplerDescription const& description);

    ID3D12DescriptorHeap* GetDescriptorHeap();

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> samplerHeap;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
