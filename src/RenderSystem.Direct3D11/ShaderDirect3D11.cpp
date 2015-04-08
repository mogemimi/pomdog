// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ShaderDirect3D11.hpp"
#include "ShaderCompiling.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static HRESULT CreateShader(ID3D11Device* device, void const* shaderByteCode,
    std::size_t bytecodeLength, ID3D11VertexShader** vertexShader)
{
    return device->CreateVertexShader(shaderByteCode, bytecodeLength, nullptr, vertexShader);
}
//-----------------------------------------------------------------------
static HRESULT CreateShader(ID3D11Device* device, void const* shaderByteCode,
    std::size_t bytecodeLength, ID3D11PixelShader** pixelShader)
{
    return device->CreatePixelShader(shaderByteCode, bytecodeLength, nullptr, pixelShader);
}

} // unnamed namespace
//-----------------------------------------------------------------------
template <class NativeShaderType>
ShaderDirect3D11<NativeShaderType>::ShaderDirect3D11(ID3D11Device* device,
    ShaderBytecode const& shaderBytecode,
    ShaderCompileOptions const& compileOptions)
{
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    if (compileOptions.Precompiled)
    {
        codeBlob.resize(shaderBytecode.ByteLength);
        std::memcpy(codeBlob.data(), shaderBytecode.Code, codeBlob.size());
    }
    else
    {
        auto compiledShaderBlob = ShaderCompiling::CompileShader(shaderBytecode, compileOptions);
        POMDOG_ASSERT(compiledShaderBlob.Get() != nullptr);

        codeBlob.resize(compiledShaderBlob->GetBufferSize());
        std::memcpy(codeBlob.data(), compiledShaderBlob->GetBufferPointer(), codeBlob.size());
    }

    POMDOG_ASSERT(device);
    POMDOG_ASSERT(!codeBlob.empty());

    HRESULT hr = CreateShader(device, codeBlob.data(), codeBlob.size(), &nativeShader);
    if (FAILED(hr)) {
        // error: FUS RO DAH!!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create shader");
    }
}
//-----------------------------------------------------------------------
template <class NativeShaderType>
NativeShaderType* ShaderDirect3D11<NativeShaderType>::GetNativeShader() const
{
    POMDOG_ASSERT(nativeShader);
    return nativeShader.Get();
}
//-----------------------------------------------------------------------
template <class NativeShaderType>
ShaderBytecode ShaderDirect3D11<NativeShaderType>::GetShaderBytecode() const
{
    POMDOG_ASSERT(!codeBlob.empty());
    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = codeBlob.data();
    shaderBytecode.ByteLength = codeBlob.size();
    return std::move(shaderBytecode);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class ShaderDirect3D11<ID3D11VertexShader>;
template class ShaderDirect3D11<ID3D11PixelShader>;
//template class ShaderDirect3D11<ID3D11GeometryShader>;
//template class ShaderDirect3D11<ID3D11DomainShader>;
//template class ShaderDirect3D11<ID3D11HullShader>;
//template class ShaderDirect3D11<ID3D11ComputeShader>;
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
