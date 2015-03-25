// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP
#define POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "ShaderDirect3D11.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;

namespace Direct3D11 {

class EffectPassDirect3D11 final: public NativeEffectPass {
public:
    EffectPassDirect3D11(ID3D11Device* device, EffectPassDescription const& description);

    std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

    void Apply(ID3D11DeviceContext * deviceContext);

    ShaderBytecode GetVertexShaderBlob() const;

    ShaderBytecode GetPixelShaderBlob() const;

private:
    std::shared_ptr<VertexShaderDirect3D11> vertexShader;
    std::shared_ptr<PixelShaderDirect3D11> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP)
