// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERDIRECT3D11_F7D61E35_F8F5_4C56_982E_279FAEE85DE4_HPP
#define POMDOG_SHADERDIRECT3D11_F7D61E35_F8F5_4C56_982E_279FAEE85DE4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include <vector>
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;
class ShaderCompileOptions;

namespace Direct3D11 {

template <class NativeShaderType>
class ShaderDirect3D11 final: public Shader {
public:
	ShaderDirect3D11(ID3D11Device* device, ShaderBytecode const& shaderBytecode,
		ShaderCompileOptions const& compileOptions);

	NativeShaderType* GetNativeShader() const;

	ShaderBytecode GetShaderBytecode() const;

private:
	Microsoft::WRL::ComPtr<NativeShaderType> nativeShader;
	std::vector<std::uint8_t> codeBlob;
};

using VertexShaderDirect3D11 = ShaderDirect3D11<ID3D11VertexShader>;
using PixelShaderDirect3D11 = ShaderDirect3D11<ID3D11PixelShader>;

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SHADERDIRECT3D11_F7D61E35_F8F5_4C56_982E_279FAEE85DE4_HPP)
