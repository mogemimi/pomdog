// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include <wrl/client.h>
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;
struct ShaderCompileOptions;

namespace Direct3D11 {

template <class NativeShaderType>
class ShaderDirect3D11 final : public Shader {
public:
    ShaderDirect3D11(
        ID3D11Device* device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);

    Microsoft::WRL::ComPtr<NativeShaderType> GetShader() const;

    ShaderBytecode GetShaderBytecode() const;

private:
    Microsoft::WRL::ComPtr<NativeShaderType> shader;
    std::vector<std::uint8_t> codeBlob;
};

using VertexShaderDirect3D11 = ShaderDirect3D11<ID3D11VertexShader>;
using PixelShaderDirect3D11 = ShaderDirect3D11<ID3D11PixelShader>;

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
