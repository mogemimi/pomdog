// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;

namespace Direct3D11 {

class EffectReflectionDirect3D11 final: public NativeEffectReflection {
public:
    EffectReflectionDirect3D11(
        ShaderBytecode const& vertexShaderBytecode,
        ShaderBytecode const& pixelShaderBytecode);

    std::vector<EffectConstantDescription> GetConstantBuffers() const override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
