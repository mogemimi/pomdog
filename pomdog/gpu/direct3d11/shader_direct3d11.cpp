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

[[nodiscard]] HRESULT
createShader(
    ID3D11Device* device,
    const void* shaderByteCode,
    std::size_t bytecodeLength,
    ID3D11VertexShader** vertexShader)
{
    return device->CreateVertexShader(shaderByteCode, bytecodeLength, nullptr, vertexShader);
}

[[nodiscard]] HRESULT
createShader(
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
ShaderDirect3D11<NativeShaderType>::initialize(
    ID3D11Device* device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(shaderBytecode.code != nullptr);
    POMDOG_ASSERT(shaderBytecode.byteLength > 0);

    if (compileOptions.precompiled) {
        codeBlob_.resize(shaderBytecode.byteLength);
        std::memcpy(codeBlob_.data(), shaderBytecode.code, codeBlob_.size());
    }
    else {
        auto [compiledShaderBlob, compileErr] = direct3d::CompileHLSL(
            shaderBytecode,
            compileOptions);

        if (compileErr != nullptr) {
            return errors::wrap(std::move(compileErr), "CompileHLSL() failed");
        }

        POMDOG_ASSERT(compiledShaderBlob.Get() != nullptr);

        codeBlob_.resize(compiledShaderBlob->GetBufferSize());
        std::memcpy(codeBlob_.data(), compiledShaderBlob->GetBufferPointer(), codeBlob_.size());
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(!codeBlob_.empty());

    if (auto hr = createShader(device, codeBlob_.data(), codeBlob_.size(), &shader_); FAILED(hr)) {
        return errors::make("CreateShader() failed: hr = " + std::to_string(hr));
    }

    return nullptr;
}

template <class NativeShaderType>
Microsoft::WRL::ComPtr<NativeShaderType>
ShaderDirect3D11<NativeShaderType>::getShader() const noexcept
{
    POMDOG_ASSERT(shader_);
    return shader_;
}

template <class NativeShaderType>
ShaderBytecode
ShaderDirect3D11<NativeShaderType>::getShaderBytecode() const noexcept
{
    POMDOG_ASSERT(!codeBlob_.empty());
    ShaderBytecode shaderBytecode;
    shaderBytecode.code = codeBlob_.data();
    shaderBytecode.byteLength = codeBlob_.size();
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
