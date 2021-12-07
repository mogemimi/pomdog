// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/shader.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D11 {

template <class NativeShaderType>
class ShaderDirect3D11 final : public Shader {
public:
    [[nodiscard]] std::unique_ptr<Error>
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
