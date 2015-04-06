// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PIPELINESTATEDIRECT3D12_13A223AC_HPP
#define POMDOG_PIPELINESTATEDIRECT3D12_13A223AC_HPP

#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

struct ConstantBufferBindDesc {
    std::string Name;
    UINT BindPoint;
};

class PipelineStateDirect3D12 final : public NativePipelineState {
public:
    PipelineStateDirect3D12(ID3D12Device* device,
        PipelineStateDescription const& description);

    std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

    ID3D12PipelineState* GetPipelineState() const noexcept;

    ID3D12RootSignature* GetRootSignature() const noexcept;

    std::size_t GetConstantBufferCount() const noexcept;

    std::size_t GetSahderResourceCount() const noexcept;

    std::size_t GetSamplerCount() const noexcept;

private:
    std::vector<ConstantBufferBindDesc> constantBufferBinds;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    std::size_t constantBufferCount;
    std::size_t shaderResourceCount;
    std::size_t samplerCount;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PIPELINESTATEDIRECT3D12_13A223AC_HPP
