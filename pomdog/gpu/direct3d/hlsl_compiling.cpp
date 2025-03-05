// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d/hlsl_compiling.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fstream>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace BinaryReader = pomdog::detail::BinaryReader;

namespace pomdog::gpu::detail::direct3d {
namespace {

[[nodiscard]] std::string
toString(const ShaderProfile& profile) noexcept
{
    std::string output;

    switch (profile.pipelineStage) {
    case ShaderPipelineStage::VertexShader:
        output += "vs_";
        break;
    case ShaderPipelineStage::PixelShader:
        output += "ps_";
        break;
        //case ShaderPipelineStage::GeometryShader:
        //    output += "gs_";
        //    break;
        //case ShaderPipelineStage::DomainShader:
        //    output += "ds_";
        //    break;
        //case ShaderPipelineStage::ComputeShader:
        //    output += "cs_";
        //    break;
        //case ShaderPipelineStage::HullShader:
        //    output += "hs_";
        //    break;
    }

    output += std::to_string(profile.shaderModel.major);
    output += '_';
    output += std::to_string(profile.shaderModel.minor);
    return output;
}

class HLSLCodeInclude final : public ID3DInclude {
private:
    std::string currentDirectory;
    std::vector<std::uint8_t> outputSource;
    std::unique_ptr<Error> lastError;

public:
    explicit HLSLCodeInclude(const std::string& curentDirectoryIn)
        : currentDirectory(curentDirectoryIn)
    {
    }

    HRESULT __stdcall Open(D3D_INCLUDE_TYPE includeType, LPCSTR pFileName, [[maybe_unused]] LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        //if (D3D10_INCLUDE_FORCE_DWORD == includeType) {
        //    return E_FAIL;
        //}

        std::string includePath;
        if (D3D_INCLUDE_LOCAL == includeType) {
            includePath = filepaths::join(currentDirectory, pFileName);
        }
        else {
            includePath = pFileName;
        }

        if (D3D_INCLUDE_LOCAL != includeType && D3D_INCLUDE_SYSTEM != includeType) {
            return E_FAIL;
        }

#if 0
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
        if (D3D_INCLUDE_LOCAL == includeType) {
            Log::Internal("ShaderInclude: Local");
        }
        else if (D3D_INCLUDE_SYSTEM == includeType) {
            Log::Internal("ShaderInclude: System");
        }
        Log::Internal(strings::format("include shader file : %s", includePath.c_str()));
#endif
#endif

        std::ifstream stream{includePath, std::ifstream::binary};

        if (!stream) {
            lastError = errors::make("Could not find a shader source file: " + includePath);
            return E_FAIL;
        }

        auto [size, err] = FileSystem::getFileSize(includePath);
        if (err != nullptr) {
            lastError = errors::make("failed to get file size: " + includePath);
            return E_FAIL;
        }

        if (size <= 0) {
            lastError = errors::make("The file is too small: " + includePath);
            return E_FAIL;
        }

        outputSource = BinaryReader::readArray<std::uint8_t>(stream, size);

        *ppData = outputSource.data();
        *pBytes = static_cast<UINT>(outputSource.size());

        return S_OK;
    }

    HRESULT __stdcall Close([[maybe_unused]] LPCVOID pData)
    {
        outputSource.clear();
        return S_OK;
    }

    [[nodiscard]] std::unique_ptr<Error>
    moveLastError() noexcept
    {
        return std::move(lastError);
    }
};

[[nodiscard]] std::unique_ptr<Error>
compileFromShaderFile(
    const ShaderBytecode& shaderBytecode,
    const std::string& entrypoint,
    const std::string& shaderProfile,
    const std::string& currentDirectory,
    unsafe_ptr<const D3D_SHADER_MACRO> preprocessorMacros,
    unsafe_ptr<unsafe_ptr<ID3DBlob>> ppBlobOut)
{
    POMDOG_ASSERT(shaderBytecode.code != nullptr);
    POMDOG_ASSERT(shaderBytecode.byteLength > 0);

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    HLSLCodeInclude shaderInclude(currentDirectory);
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    const auto hr = ::D3DCompile(
        shaderBytecode.code,
        shaderBytecode.byteLength,
        nullptr,
        preprocessorMacros,
        &shaderInclude,
        entrypoint.c_str(),
        shaderProfile.c_str(),
        shaderFlags,
        0,
        ppBlobOut,
        &errorBlob);

    if (auto err = shaderInclude.moveLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to compile shader");
    }

    if (FAILED(hr)) {
        if (errorBlob != nullptr) {
            std::string str(reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
            return errors::make("failed to compile shader. error: " + str);
        }
        return errors::make("D3DCompile() failed. HRESULT = " + std::to_string(hr));
    }

    if (errorBlob != nullptr) {
        std::string str(reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
        return errors::make("warning: " + str);
    }

    return nullptr;
}

} // namespace

std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, std::unique_ptr<Error>>
CompileHLSL(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    const auto target = toString(compileOptions.profile);
    POMDOG_ASSERT(!target.empty());

    std::vector<D3D_SHADER_MACRO> defines;
    defines.reserve(compileOptions.preprocessorMacros.size());

    for (auto& macro : compileOptions.preprocessorMacros) {
        if (macro.name.empty()) {
            return std::make_tuple(nullptr, errors::make("macro.Name is empty"));
        }

        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = macro.name.c_str();
        shaderMacro.Definition = macro.definition.empty() ? nullptr : macro.definition.c_str();
        defines.push_back(std::move(shaderMacro));
    }

    if (!defines.empty()) {
        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = nullptr;
        shaderMacro.Definition = nullptr;
        defines.push_back(std::move(shaderMacro));
    }

    Microsoft::WRL::ComPtr<ID3DBlob> codeBlob;
    if (auto err = compileFromShaderFile(shaderBytecode,
            compileOptions.entryPoint,
            target,
            compileOptions.currentDirectory,
            (defines.empty() ? nullptr : defines.data()),
            &codeBlob);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CompileFromShaderFile() failed"));
    }

    return std::make_tuple(std::move(codeBlob), nullptr);
}

} // namespace pomdog::gpu::detail::direct3d
