// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "HLSLCompiling.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Direct3D {
namespace {

std::string ToString(const ShaderProfile& profile)
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

    return std::move(output);
}

class HLSLCodeInclude : public ID3DInclude {
private:
    std::string currentDirectory;
    std::vector<std::uint8_t> outputSource;

public:
    explicit HLSLCodeInclude(const std::string& curentDirectoryIn)
        : currentDirectory(curentDirectoryIn)
    {
    }

    HRESULT __stdcall Open(D3D_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        UNREFERENCED_PARAMETER(pParentData);

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

        if (D3D_INCLUDE_LOCAL != includeType
            && D3D_INCLUDE_SYSTEM != includeType) {
            return E_FAIL;
        }

#if defined(DEBUG) && !defined(NDEBUG)
        if (D3D_INCLUDE_LOCAL == includeType) {
            Log::Internal("ShaderInclude: Local");
        }
        else if (D3D_INCLUDE_SYSTEM == includeType) {
            Log::Internal("ShaderInclude: System");
        }
        Log::Internal(StringHelper::Format("include shader file : %s", includePath.c_str()));
#endif

        auto binaryFile = PathHelper::OpenStream(includePath);

        if (!binaryFile.Stream) {
            Log::Internal(StringHelper::Format("Could not find a shader source file %s", includePath.c_str()));
            return E_FAIL;
        }

        if (binaryFile.SizeInBytes <= 0) {
            Log::Internal(StringHelper::Format("The file is too small %s", includePath.c_str()));
            return E_FAIL;
        }

        outputSource = BinaryReader::ReadString<std::uint8_t>(
            binaryFile.Stream, binaryFile.SizeInBytes);

        *ppData = outputSource.data();
        *pBytes = static_cast<UINT>(outputSource.size());

        return S_OK;
    }

    HRESULT __stdcall Close(LPCVOID pData)
    {
        UNREFERENCED_PARAMETER(pData);
        outputSource.clear();
        return S_OK;
    }
};

void CompileFromShaderFile(
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

    HRESULT hr = ::D3DCompile(shaderBytecode.Code, shaderBytecode.ByteLength,
        nullptr, preprocessorMacros, &shaderInclude,
        entrypoint.c_str(), shaderProfile.c_str(), shaderFlags, 0, ppBlobOut, &errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, StringHelper::Format(
                "Failed to compile shader.\n"
                "error: %s",
                reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer())));
        }
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to compile shader.");
    }

    if (errorBlob) {
        Log::Internal(StringHelper::Format("warning (compile shader): %s",
            reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer())));
    }
}

} // unnamed namespace

Microsoft::WRL::ComPtr<ID3DBlob> HLSLCompiling::CompileShader(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    const auto target = ToString(compileOptions.Profile);
    POMDOG_ASSERT(!target.empty());

    std::vector<D3D_SHADER_MACRO> defines;
    defines.reserve(compileOptions.PreprocessorMacros.size());

    for (auto& macro : compileOptions.PreprocessorMacros) {
        if (macro.Name.empty()) {
            continue;
        }

        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = macro.Name.c_str();
        shaderMacro.Definition = macro.Definition.empty() ? nullptr: macro.Definition.c_str();
        defines.push_back(std::move(shaderMacro));
    }

    if (!defines.empty()) {
        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = nullptr;
        shaderMacro.Definition = nullptr;
        defines.push_back(std::move(shaderMacro));
    }

    Microsoft::WRL::ComPtr<ID3DBlob> codeBlob;
    CompileFromShaderFile(shaderBytecode,
        compileOptions.EntryPoint,
        target,
        compileOptions.CurrentDirectory,
        (defines.empty() ? nullptr: defines.data()),
        &codeBlob);

    return std::move(codeBlob);
}

} // namespace Direct3D
} // namespace Detail
} // namespace Pomdog
