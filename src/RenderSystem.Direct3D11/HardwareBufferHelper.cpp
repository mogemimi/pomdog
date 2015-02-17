//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "HardwareBufferHelper.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

struct BufferDescription {
	D3D11_USAGE Usage;
	UINT CPUAccessFlags;
};

static BufferDescription ToDescription(BufferUsage bufferUsage)
{
	switch (bufferUsage) {
	case BufferUsage::Dynamic:
		return { D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
	case BufferUsage::Immutable:
		return { D3D11_USAGE_IMMUTABLE, 0 };
	}
	return { D3D11_USAGE_DEFAULT, 0 };
}
//-----------------------------------------------------------------------
static ID3D11Buffer* CreateNativeBuffer(ID3D11Device* nativeDevice,
	std::size_t byteWidth, void const* data, BufferUsage bufferUsage, UINT bindFlags)
{
	const auto description = ToDescription(bufferUsage);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = description.Usage;
	bufferDesc.ByteWidth = static_cast<::UINT>(byteWidth);
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.CPUAccessFlags = description.CPUAccessFlags;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = data;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	POMDOG_ASSERT(nativeDevice);
	POMDOG_ASSERT((bufferDesc.Usage != D3D11_USAGE_IMMUTABLE)
		|| ((bufferDesc.Usage == D3D11_USAGE_IMMUTABLE) && (data != nullptr)));

	auto initialData = (data != nullptr) ? &subresourceData : nullptr;

	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = nativeDevice->CreateBuffer(&bufferDesc, initialData, &buffer);

	if (FAILED(hr))
	{
		///@error FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create ID3D11Buffer");
	}

	return std::move(buffer);
}

}// unnamed namespace
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11Buffer>
HardwareBufferHelper::CreateVertexBuffer(ID3D11Device* nativeDevice,
	void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage)
{
	return CreateNativeBuffer(nativeDevice, sizeInBytes, data, bufferUsage,
		D3D11_BIND_VERTEX_BUFFER);
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11Buffer>
HardwareBufferHelper::CreateIndexBuffer(ID3D11Device* nativeDevice,
	void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage)
{
	return CreateNativeBuffer(nativeDevice, sizeInBytes, data, bufferUsage,
		D3D11_BIND_INDEX_BUFFER);
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11Buffer>
HardwareBufferHelper::CreateConstantBuffer(ID3D11Device* nativeDevice,
	void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage)
{
	POMDOG_ASSERT_MESSAGE(sizeInBytes % 16 == 0, "You must set the sizeInBytes value in multiples of 16.");
	POMDOG_ASSERT_MESSAGE(sizeInBytes <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT,
		"You must set the sizeInBytes value less than or equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.");

	return CreateNativeBuffer(nativeDevice, sizeInBytes, data, bufferUsage,
		D3D11_BIND_CONSTANT_BUFFER);
}
//-----------------------------------------------------------------------
void HardwareBufferHelper::SetData(ID3D11Buffer* buffer, ID3D11DeviceContext* deviceContext,
	std::size_t offsetInBytes, void const* source, std::size_t sizeInBytes)
{
	POMDOG_ASSERT(buffer);
	POMDOG_ASSERT(deviceContext);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto hr = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		///@error FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
	}

	POMDOG_ASSERT(source);
	POMDOG_ASSERT(sizeInBytes > 0);
	auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData) + offsetInBytes;
	std::memcpy(mappedMemory, source, sizeInBytes);

	deviceContext->Unmap(buffer, 0);
}
//-----------------------------------------------------------------------
void HardwareBufferHelper::GetData(ID3D11Buffer* buffer, ID3D11DeviceContext* deviceContext,
	std::size_t offsetInBytes, std::size_t sizeInBytes, void* output)
{
	POMDOG_ASSERT(buffer);
	POMDOG_ASSERT(deviceContext);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto hr = deviceContext->Map(buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (FAILED(hr))
	{
		///@error FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
	}

	POMDOG_ASSERT(output);
	POMDOG_ASSERT(sizeInBytes > 0);
	auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData) + offsetInBytes;
	std::memcpy(output, mappedMemory, sizeInBytes);

	deviceContext->Unmap(buffer, 0);
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
