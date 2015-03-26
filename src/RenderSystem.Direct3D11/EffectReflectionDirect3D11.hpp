// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_HPP
#define POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;

namespace Direct3D11 {

class EffectReflectionDirect3D11 final: public NativeEffectReflection {
public:
    EffectReflectionDirect3D11(ShaderBytecode const& vertexShaderBytecode,
        ShaderBytecode const& pixelShaderBytecode);

    std::vector<EffectConstantDescription> GetConstantBuffers() const override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_HPP
