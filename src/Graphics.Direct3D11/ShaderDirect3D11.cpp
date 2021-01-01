// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "ShaderDirect3D11.hpp"
#include "../Graphics.Backends/ShaderBytecode.hpp"
#include "../Graphics.Backends/ShaderCompileOptions.hpp"
#include "../Graphics.Direct3D/HLSLCompiling.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {
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
        auto [compiledShaderBlob, compileErr] = Direct3D::CompileHLSL(
            shaderBytecode,
            compileOptions);

        if (compileErr != nullptr) {
            return Errors::Wrap(std::move(compileErr), "CompileHLSL() failed");
        }

        POMDOG_ASSERT(compiledShaderBlob.Get() != nullptr);

        codeBlob.resize(compiledShaderBlob->GetBufferSize());
        std::memcpy(codeBlob.data(), compiledShaderBlob->GetBufferPointer(), codeBlob.size());
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(!codeBlob.empty());

    if (auto hr = CreateShader(device, codeBlob.data(), codeBlob.size(), &shader); FAILED(hr)) {
        return Errors::New("CreateShader() failed: hr = " + std::to_string(hr));
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

} // namespace Pomdog::Detail::Direct3D11
