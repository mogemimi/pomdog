// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/shader_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

[[nodiscard]] HRESULT
createShader(
    unsafe_ptr<ID3D11Device> device,
    const void* shaderByteCode,
    std::size_t bytecodeLength,
    ID3D11VertexShader** vertexShader)
{
    return device->CreateVertexShader(shaderByteCode, bytecodeLength, nullptr, vertexShader);
}

[[nodiscard]] HRESULT
createShader(
    unsafe_ptr<ID3D11Device> device,
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
    unsafe_ptr<ID3D11Device> device,
    std::span<const u8> shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    if (shaderBytecode.data() == nullptr || shaderBytecode.size() == 0) {
        return errors::make("shader bytecode is empty.");
    }

    if (compileOptions.precompiled) {
        codeBlob_.resize(shaderBytecode.size());
        std::memcpy(codeBlob_.data(), shaderBytecode.data(), codeBlob_.size());
    }
    else {
        return errors::make("shader bytecode is not precompiled.");
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
std::span<const u8>
ShaderDirect3D11<NativeShaderType>::getShaderBytecode() const noexcept
{
    POMDOG_ASSERT(!codeBlob_.empty());
    return std::span<const u8>(codeBlob_.data(), codeBlob_.size());
}

// NOTE: explicit instantiations
template class ShaderDirect3D11<ID3D11VertexShader>;
template class ShaderDirect3D11<ID3D11PixelShader>;
// template class ShaderDirect3D11<ID3D11GeometryShader>;
// template class ShaderDirect3D11<ID3D11DomainShader>;
// template class ShaderDirect3D11<ID3D11HullShader>;
// template class ShaderDirect3D11<ID3D11ComputeShader>;

} // namespace pomdog::gpu::detail::direct3d11
