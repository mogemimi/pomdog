//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "ShaderCompiling.hpp"
#include "../Utility/PathHelper.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>
#include <fstream>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static std::string ToString(ShaderProfile const& profile)
{
	std::string output;
	switch (profile.ProgramType) {
	case ShaderProgramType::VertexShader:
		output += "vs_";
		break;
	case ShaderProgramType::PixelShader:
		output += "ps_";
		break;
	case ShaderProgramType::GeometryShader:
		output += "gs_";
		break;
	case ShaderProgramType::DomainShader:
		output += "ds_";
		break;
	case ShaderProgramType::ComputeShader:
		output += "cs_";
		break;
	case ShaderProgramType::HullShader:
		output += "hs_";
		break;
	}

	switch (profile.ShaderModel) {
	case ShaderModel::SM_2_0:
		output += "2_0";
		break;
	case ShaderModel::SM_3_0:
		output += "3_0";
		break;
	case ShaderModel::SM_4_0:
		output += "4_0";
		break;
	case ShaderModel::SM_5_0:
		output += "5_0";
		break;
	}
	return std::move(output);
}
//-----------------------------------------------------------------------
static std::vector<std::uint8_t> ReadBinaryFile(std::ifstream && streamIn)
{
	std::ifstream stream = std::move(streamIn);
	POMDOG_ASSERT(stream);

	stream.seekg(0, stream.end);
	auto const length = static_cast<std::size_t>(stream.tellg());
	stream.seekg(0, stream.beg);

	std::vector<std::uint8_t> result(length + 1, 0);
	stream.read(reinterpret_cast<char*>(result.data()), result.size());
	return std::move(result);
}
//-----------------------------------------------------------------------
class HLSLCodeInclude : public ID3D10Include {
private:
	std::string currentDirectory;
	std::vector<std::uint8_t> outputSource;

public:
	explicit HLSLCodeInclude(std::string const& curentDirectoryIn)
		: currentDirectory(curentDirectoryIn)
	{}

	HRESULT __stdcall Open(D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		UNREFERENCED_PARAMETER(pParentData);

		//if (D3D10_INCLUDE_FORCE_DWORD == includeType) {
		//	return E_FAIL;
		//}

		std::string includePath;
		if (D3D10_INCLUDE_LOCAL == includeType) {
			includePath = PathHelper::Join(currentDirectory, pFileName);
		}
		else {
			includePath = pFileName;
		}

		if (D3D10_INCLUDE_LOCAL != includeType
			&& D3D10_INCLUDE_SYSTEM != includeType) {
			return E_FAIL;
		}

#if defined(DEBUG) && !defined(NDEBUG)
		if (D3D10_INCLUDE_LOCAL == includeType) {
			Log::Internal("ShaderInclude: Local");
		}
		else if (D3D10_INCLUDE_SYSTEM == includeType) {
			Log::Internal("ShaderInclude: System");
		}
		Log::Internal(StringFormat("include shader file : %s", includePath.c_str()));
#endif

		std::ifstream stream(includePath);

		if (stream) {
			Log::Internal(StringFormat("Could not find a shader source file %s", includePath.c_str()));
			return E_FAIL;
		}

		outputSource = ReadBinaryFile(std::move(stream));

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
//-----------------------------------------------------------------------
static void CompileFromShaderFile(std::string const& source,
	std::string const& shaderProfile, std::string const& entrypoint,
	std::string const& currentDirectory, ID3DBlob** ppBlobOut)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) && !defined(NDEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	HLSLCodeInclude shaderInclude(currentDirectory);
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = ::D3DCompile(source.c_str(), source.size(), nullptr, nullptr, &shaderInclude,
		entrypoint.c_str(), shaderProfile.c_str(), shaderFlags, 0, ppBlobOut, &errorBlob);

	if (FAILED(hr))
	{
		///@todo throw exception
		// error, FUS RO DAH!

		if (errorBlob) {
			Log::Internal(StringFormat(
				"Failed to compile shader. The HLSL file cannot be compiled.\nerror: %s",
				reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer())));
		}
		return;
	}

	if (errorBlob) {
		Log::Internal(StringFormat("warning (compile shader): %s",
			reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer())));
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
std::vector<std::uint8_t> ShaderCompiling::CompileShader(std::string const& source,
	ShaderProfile const& shaderProfile, std::string const& entrypoint,
	std::string const& currentDirectory)
{
	const auto target = ToString(shaderProfile);
	POMDOG_ASSERT(!target.empty());

	Microsoft::WRL::ComPtr<ID3DBlob> codeBlob;
	CompileFromShaderFile(source, target, entrypoint, currentDirectory, &codeBlob);

	POMDOG_ASSERT(codeBlob);
	std::vector<std::uint8_t> shaderBytecode(codeBlob->GetBufferSize());
	std::memcpy(shaderBytecode.data(), codeBlob->GetBufferPointer(), shaderBytecode.size());

	return std::move(shaderBytecode);
}

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
