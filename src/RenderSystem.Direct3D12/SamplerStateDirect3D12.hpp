// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATEDIRECT3D12_28639E1F_HPP
#define POMDOG_SAMPLERSTATEDIRECT3D12_28639E1F_HPP

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

#endif // POMDOG_SAMPLERSTATEDIRECT3D12_28639E1F_HPP
