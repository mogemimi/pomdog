// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTREFLECTIONDIRECT3D12_87A4D3AB_HPP
#define POMDOG_EFFECTREFLECTIONDIRECT3D12_87A4D3AB_HPP

#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <d3d12shader.h>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;

namespace Direct3D12 {

class EffectReflectionDirect3D12 final : public NativeEffectReflection {
public:
    EffectReflectionDirect3D12(
        ShaderBytecode const& vertexShaderBytecode,
        ShaderBytecode const& pixelShaderBytecode);

    std::vector<EffectConstantDescription> GetConstantBuffers() const override;

private:
    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> vertexShaderReflector;
    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> pixelShaderReflector;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_EFFECTREFLECTIONDIRECT3D12_87A4D3AB_HPP
