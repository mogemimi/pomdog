//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "EffectPassDirect3D11.hpp"
#include "ConstantLayoutDirect3D11.hpp"
#include "GraphicsDeviceDirect3D11.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static void ReflectShaderBytecode(ShaderBytecode const& shaderBytecode,
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> & shaderReflector, D3D11_SHADER_DESC & shaderDesc)
{
	HRESULT hr = D3DReflect(shaderBytecode.Code, shaderBytecode.ByteLength,
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflector.GetAddressOf()));

	if (FAILED(hr))
	{
		// FUS RO DAH!!
		///@todo throw exception
	}

	POMDOG_ASSERT(shaderReflector);
	hr = shaderReflector->GetDesc(&shaderDesc);

	if (FAILED(hr))
	{
		// FUS RO DAH!!
		///@todo throw exception
	}
}
//-----------------------------------------------------------------------
static void EnumerateConstantBuffers(
	ShaderBytecode const& shaderBytecode,
	std::vector<ConstantBufferBindingDirect3D11> & output)
{
	POMDOG_ASSERT(shaderBytecode.Code);

	D3D11_SHADER_DESC shaderDesc;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

	ReflectShaderBytecode(shaderBytecode, shaderReflector, shaderDesc);

	for (std::size_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		POMDOG_ASSERT(shaderReflector);
		auto constantBufferReflector = shaderReflector->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC bufferDesc;
		HRESULT hr = constantBufferReflector->GetDesc(&bufferDesc);

		if (FAILED(hr))
		{
			// FUS RO DAH!!
			///@todo throw exception
			continue;
		}

		//if (D3D11_CT_CBUFFER != constantBufferDesc.Type)
		//{
		//	///@todo Not implemented
		//	//tbuffer
		//	//continue;
		//}

		D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
		shaderReflector->GetResourceBindingDescByName(bufferDesc.Name, &shaderInputBindDesc);

		ConstantBufferBindingDirect3D11 binding;
		binding.Name = bufferDesc.Name;
		binding.StartSlot = static_cast<std::uint16_t>(shaderInputBindDesc.BindPoint);

		POMDOG_ASSERT(shaderInputBindDesc.BindPoint < std::numeric_limits<decltype(binding.StartSlot)>::max());
		POMDOG_ASSERT(shaderInputBindDesc.BindPoint < D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);

		output.push_back(std::move(binding));
	}
}
//-----------------------------------------------------------------------
static std::vector<ConstantBufferBindingDirect3D11> CreateConstantBufferBindings(
	ShaderBytecode const& vertexShaderBytecode,
	ShaderBytecode const& pixelShaderBytecode)
{
	using Desc = ConstantBufferBindingDirect3D11;

	std::vector<Desc> bindings;
	EnumerateConstantBuffers(vertexShaderBytecode, bindings);
	EnumerateConstantBuffers(pixelShaderBytecode, bindings);

	auto equal = [](Desc const& a, Desc const& b) {
		return a.Name == b.Name;
	};

	std::sort(std::begin(bindings), std::end(bindings), equal);
	bindings.erase(std::unique(std::begin(bindings), std::end(bindings), equal), std::end(bindings));
	bindings.shrink_to_fit();

	return std::move(bindings);
}

}// unnamed namespace
//-----------------------------------------------------------------------
EffectPassDirect3D11::EffectPassDirect3D11(ID3D11Device * device,
	ShaderBytecode const& vertexShaderBytecode,
	ShaderBytecode const& pixelShaderBytecode)
{
	POMDOG_ASSERT(device);

	// Create the vertex shader
	HRESULT hr = device->CreateVertexShader(vertexShaderBytecode.Code,
		vertexShaderBytecode.ByteLength, nullptr, &vertexShader);

	if (FAILED(hr)) {
		// FUS RO DAH!!
		///@todo throw exception
		return;
	}

	// Create the pixel shader
	hr = device->CreatePixelShader(pixelShaderBytecode.Code,
		pixelShaderBytecode.ByteLength, nullptr, &pixelShader);

	if (FAILED(hr)) {
		// FUS RO DAH!!
		// throw exception
		return;
	}

	vertexShaderBlob.resize(vertexShaderBytecode.ByteLength);
	std::memcpy(vertexShaderBlob.data(), vertexShaderBytecode.Code, vertexShaderBlob.size());

	pixelShaderBlob.resize(pixelShaderBytecode.ByteLength);
	std::memcpy(pixelShaderBlob.data(), pixelShaderBytecode.Code, pixelShaderBlob.size());
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeConstantLayout> EffectPassDirect3D11::CreateConstantLayout()
{
	ShaderBytecode vertexShaderBytecode;
	vertexShaderBytecode.Code = vertexShaderBlob.data();
	vertexShaderBytecode.ByteLength = vertexShaderBlob.size();

	ShaderBytecode pixelShaderBytecode;
	pixelShaderBytecode.Code = pixelShaderBlob.data();
	pixelShaderBytecode.ByteLength = pixelShaderBlob.size();

	auto bindings = CreateConstantBufferBindings(vertexShaderBytecode, pixelShaderBytecode);

	auto constantLayout = std::make_unique<ConstantLayoutDirect3D11>(std::move(bindings));
	return std::move(constantLayout);
}
//-----------------------------------------------------------------------
void EffectPassDirect3D11::Apply(ID3D11DeviceContext * deviceContext)
{
	POMDOG_ASSERT(deviceContext);
	POMDOG_ASSERT(vertexShader);
	POMDOG_ASSERT(pixelShader);

	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
}
//-----------------------------------------------------------------------
ShaderBytecode EffectPassDirect3D11::GetVertexShaderBlob() const
{
	POMDOG_ASSERT(!vertexShaderBlob.empty());
	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = vertexShaderBlob.data();
	shaderBytecode.ByteLength = vertexShaderBlob.size();
	return std::move(shaderBytecode);
}
//-----------------------------------------------------------------------
ShaderBytecode EffectPassDirect3D11::GetPixelShaderBlob() const
{
	POMDOG_ASSERT(!pixelShaderBlob.empty());
	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = pixelShaderBlob.data();
	shaderBytecode.ByteLength = pixelShaderBlob.size();
	return std::move(shaderBytecode);
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
