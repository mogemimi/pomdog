// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
