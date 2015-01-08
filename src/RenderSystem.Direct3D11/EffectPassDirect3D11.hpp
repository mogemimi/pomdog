//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP
#define POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {
namespace Direct3D11 {

class EffectPassDirect3D11 final: public NativeEffectPass {
public:
	EffectPassDirect3D11(ID3D11Device * device,
		ShaderBytecode const& vertexShaderBytecode,
		ShaderBytecode const& pixelShaderBytecode);

	std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

	void Apply(ID3D11DeviceContext * deviceContext);

	ShaderBytecode GetVertexShaderBlob() const;

	ShaderBytecode GetPixelShaderBlob() const;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	std::vector<std::uint8_t> vertexShaderBlob;
	std::vector<std::uint8_t> pixelShaderBlob;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSDIRECT3D11_0A0A09ED_D3D3_4E70_B836_D24811A8FA94_HPP)
