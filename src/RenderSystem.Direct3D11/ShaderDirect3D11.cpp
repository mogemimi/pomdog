// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ShaderDirect3D11.hpp"
#include "../RenderSystem.Direct3D/HLSLCompiling.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {
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

} // unnamed namespace

template <class NativeShaderType>
ShaderDirect3D11<NativeShaderType>::ShaderDirect3D11(
    ID3D11Device* device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    if (compileOptions.Precompiled) {
        codeBlob.resize(shaderBytecode.ByteLength);
        std::memcpy(codeBlob.data(), shaderBytecode.Code, codeBlob.size());
    }
    else {
        auto compiledShaderBlob = Direct3D::HLSLCompiling::CompileShader(
            shaderBytecode, compileOptions);
        POMDOG_ASSERT(compiledShaderBlob.Get() != nullptr);

        codeBlob.resize(compiledShaderBlob->GetBufferSize());
        std::memcpy(codeBlob.data(), compiledShaderBlob->GetBufferPointer(), codeBlob.size());
    }

    POMDOG_ASSERT(device);
    POMDOG_ASSERT(!codeBlob.empty());

    HRESULT hr = CreateShader(device, codeBlob.data(), codeBlob.size(), &shader);
    if (FAILED(hr)) {
        // error: FUS RO DAH!!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create shader");
    }
}

template <class NativeShaderType>
Microsoft::WRL::ComPtr<NativeShaderType> ShaderDirect3D11<NativeShaderType>::GetShader() const
{
    POMDOG_ASSERT(shader);
    return shader;
}

template <class NativeShaderType>
ShaderBytecode ShaderDirect3D11<NativeShaderType>::GetShaderBytecode() const
{
    POMDOG_ASSERT(!codeBlob.empty());
    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = codeBlob.data();
    shaderBytecode.ByteLength = codeBlob.size();
    return std::move(shaderBytecode);
}

// explicit instantiations
template class ShaderDirect3D11<ID3D11VertexShader>;
template class ShaderDirect3D11<ID3D11PixelShader>;
//template class ShaderDirect3D11<ID3D11GeometryShader>;
//template class ShaderDirect3D11<ID3D11DomainShader>;
//template class ShaderDirect3D11<ID3D11HullShader>;
//template class ShaderDirect3D11<ID3D11ComputeShader>;

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
