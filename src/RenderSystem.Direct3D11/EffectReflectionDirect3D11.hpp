// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail {
class ShaderBytecode;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D11 {

class EffectReflectionDirect3D11 final : public EffectReflection {
public:
    EffectReflectionDirect3D11(
        const ShaderBytecode& vertexShaderBytecode,
        const ShaderBytecode& pixelShaderBytecode);

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector;
};

} // namespace Pomdog::Detail::Direct3D11
