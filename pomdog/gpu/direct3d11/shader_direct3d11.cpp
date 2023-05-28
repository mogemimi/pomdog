// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/shader_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/direct3d/hlsl_compiling.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

HRESULT CreateShader(
    ID3D11Device* device,
    const void* shaderByteCode,
    std::size_t bytecodeLength,
    ID3D11VertexShader** vertexShader)
{
    return device->CreateVertexShader(shaderByteCode, bytecodeLength, nullptr, vertexShader);
}

HRESULT CreateShader(
    ID3D11Device* device,
    const void* shaderByteCode,
    std::size_t bytecodeLength,
    ID3D11PixelShader** pixelShader)
{
    return device->CreatePixelShader(shaderByteCode, bytecodeLength, nullptr, pixelShader);
}

} // namespace

template <class NativeShaderType>
std::unique_ptr<Error>
ShaderDirect3D11<NativeShaderType>::Initialize(
    ID3D11Device* device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    if (compileOptions.Precompiled) {
        codeBlob.resize(shaderBytecode.ByteLength);
        std::memcpy(codeBlob.data(), shaderBytecode.Code, codeBlob.size());
    }
    else {
        auto [compiledShaderBlob, compileErr] = direct3d::CompileHLSL(
            shaderBytecode,
            compileOptions);

        if (compileErr != nullptr) {
            return errors::wrap(std::move(compileErr), "CompileHLSL() failed");
        }

        POMDOG_ASSERT(compiledShaderBlob.Get() != nullptr);

        codeBlob.resize(compiledShaderBlob->GetBufferSize());
        std::memcpy(codeBlob.data(), compiledShaderBlob->GetBufferPointer(), codeBlob.size());
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(!codeBlob.empty());

    if (auto hr = CreateShader(device, codeBlob.data(), codeBlob.size(), &shader); FAILED(hr)) {
        return errors::make("CreateShader() failed: hr = " + std::to_string(hr));
    }

    return nullptr;
}

template <class NativeShaderType>
Microsoft::WRL::ComPtr<NativeShaderType> ShaderDirect3D11<NativeShaderType>::GetShader() const noexcept
{
    POMDOG_ASSERT(shader);
    return shader;
}

template <class NativeShaderType>
ShaderBytecode ShaderDirect3D11<NativeShaderType>::GetShaderBytecode() const noexcept
{
    POMDOG_ASSERT(!codeBlob.empty());
    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = codeBlob.data();
    shaderBytecode.ByteLength = codeBlob.size();
    return std::move(shaderBytecode);
}

// NOTE: explicit instantiations
template class ShaderDirect3D11<ID3D11VertexShader>;
template class ShaderDirect3D11<ID3D11PixelShader>;
// template class ShaderDirect3D11<ID3D11GeometryShader>;
// template class ShaderDirect3D11<ID3D11DomainShader>;
// template class ShaderDirect3D11<ID3D11HullShader>;
// template class ShaderDirect3D11<ID3D11ComputeShader>;

} // namespace pomdog::gpu::detail::direct3d11
