// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/direct3d/hlsl_compiling.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/content/utility/binary_reader.hpp"
#include "pomdog/filesystem/file_system.hpp"
#include "pomdog/graphics/backends/shader_bytecode.hpp"
#include "pomdog/graphics/backends/shader_compile_options.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/path_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fstream>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::direct3d {
namespace {

[[nodiscard]] std::string
ToString(const ShaderProfile& profile) noexcept
{
    std::string output;

    switch (profile.PipelineStage) {
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

    output += std::to_string(profile.ShaderModel.Major);
    output += '_';
    output += std::to_string(profile.ShaderModel.Minor);
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
            includePath = PathHelper::Join(currentDirectory, pFileName);
        }
        else {
            includePath = pFileName;
        }

        if (D3D_INCLUDE_LOCAL != includeType && D3D_INCLUDE_SYSTEM != includeType) {
            return E_FAIL;
        }

#if 0
#if defined(DEBUG) && !defined(NDEBUG)
        if (D3D_INCLUDE_LOCAL == includeType) {
            Log::Internal("ShaderInclude: Local");
        }
        else if (D3D_INCLUDE_SYSTEM == includeType) {
            Log::Internal("ShaderInclude: System");
        }
        Log::Internal(StringHelper::Format("include shader file : %s", includePath.c_str()));
#endif
#endif

        std::ifstream stream{includePath, std::ifstream::binary};

        if (!stream) {
            lastError = errors::New("Could not find a shader source file: " + includePath);
            return E_FAIL;
        }

        auto [size, err] = FileSystem::GetFileSize(includePath);
        if (err != nullptr) {
            lastError = errors::New("failed to get file size: " + includePath);
            return E_FAIL;
        }

        if (size <= 0) {
            lastError = errors::New("The file is too small: " + includePath);
            return E_FAIL;
        }

        outputSource = BinaryReader::ReadArray<std::uint8_t>(stream, size);

        *ppData = outputSource.data();
        *pBytes = static_cast<UINT>(outputSource.size());

        return S_OK;
    }

    HRESULT __stdcall Close([[maybe_unused]] LPCVOID pData)
    {
        outputSource.clear();
        return S_OK;
    }

    std::unique_ptr<Error>
    MoveLastError() noexcept
    {
        return std::move(lastError);
    }
};

[[nodiscard]] std::unique_ptr<Error>
CompileFromShaderFile(
    const ShaderBytecode& shaderBytecode,
    const std::string& entrypoint,
    const std::string& shaderProfile,
    const std::string& currentDirectory,
    const D3D_SHADER_MACRO* preprocessorMacros,
    ID3DBlob** ppBlobOut)
{
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) && !defined(NDEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    HLSLCodeInclude shaderInclude(currentDirectory);
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    const auto hr = ::D3DCompile(
        shaderBytecode.Code,
        shaderBytecode.ByteLength,
        nullptr,
        preprocessorMacros,
        &shaderInclude,
        entrypoint.c_str(),
        shaderProfile.c_str(),
        shaderFlags,
        0,
        ppBlobOut,
        &errorBlob);

    if (auto err = shaderInclude.MoveLastError(); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to compile shader");
    }

    if (FAILED(hr)) {
        if (errorBlob != nullptr) {
            std::string str(reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
            return errors::New("failed to compile shader. error: " + str);
        }
        return errors::New("D3DCompile() failed. HRESULT = " + std::to_string(hr));
    }

    if (errorBlob != nullptr) {
        std::string str(reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
        return errors::New("warning: " + str);
    }

    return nullptr;
}

} // namespace

std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, std::unique_ptr<Error>>
CompileHLSL(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    const auto target = ToString(compileOptions.Profile);
    POMDOG_ASSERT(!target.empty());

    std::vector<D3D_SHADER_MACRO> defines;
    defines.reserve(compileOptions.PreprocessorMacros.size());

    for (auto& macro : compileOptions.PreprocessorMacros) {
        if (macro.Name.empty()) {
            return std::make_tuple(nullptr, errors::New("macro.Name is empty"));
        }

        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = macro.Name.c_str();
        shaderMacro.Definition = macro.Definition.empty() ? nullptr : macro.Definition.c_str();
        defines.push_back(std::move(shaderMacro));
    }

    if (!defines.empty()) {
        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = nullptr;
        shaderMacro.Definition = nullptr;
        defines.push_back(std::move(shaderMacro));
    }

    Microsoft::WRL::ComPtr<ID3DBlob> codeBlob;
    if (auto err = CompileFromShaderFile(shaderBytecode,
            compileOptions.EntryPoint,
            target,
            compileOptions.CurrentDirectory,
            (defines.empty() ? nullptr : defines.data()),
            &codeBlob);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CompileFromShaderFile() failed"));
    }

    return std::make_tuple(std::move(codeBlob), nullptr);
}

} // namespace pomdog::detail::direct3d
