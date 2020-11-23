// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <wrl/client.h>
#include <cstdint>
#include <vector>

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D11 {

template <class NativeShaderType>
class ShaderDirect3D11 final : public Shader {
public:
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        ID3D11Device* device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions) noexcept;

    /// Gets the pointer of the native shader object.
    [[nodiscard]] Microsoft::WRL::ComPtr<NativeShaderType>
    GetShader() const noexcept;

    /// Gets the pointer of the shader bytecode.
    [[nodiscard]] ShaderBytecode
    GetShaderBytecode() const noexcept;

private:
    Microsoft::WRL::ComPtr<NativeShaderType> shader;
    std::vector<std::uint8_t> codeBlob;
};

using VertexShaderDirect3D11 = ShaderDirect3D11<ID3D11VertexShader>;
using PixelShaderDirect3D11 = ShaderDirect3D11<ID3D11PixelShader>;

} // namespace Pomdog::Detail::Direct3D11
