// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERDIRECT3D11_F7D61E35_HPP
#define POMDOG_SHADERDIRECT3D11_F7D61E35_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include <wrl/client.h>
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
    ShaderDirect3D11(
        ID3D11Device* device,
        ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions);

    Microsoft::WRL::ComPtr<NativeShaderType> GetShader() const;

    ShaderBytecode GetShaderBytecode() const;

private:
    Microsoft::WRL::ComPtr<NativeShaderType> shader;
    std::vector<std::uint8_t> codeBlob;
};

using VertexShaderDirect3D11 = ShaderDirect3D11<ID3D11VertexShader>;
using PixelShaderDirect3D11 = ShaderDirect3D11<ID3D11PixelShader>;

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SHADERDIRECT3D11_F7D61E35_HPP
