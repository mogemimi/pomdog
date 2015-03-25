// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "IndexBufferDirect3D11.hpp"
#include "HardwareBufferHelper.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
//-----------------------------------------------------------------------
IndexBufferDirect3D11::IndexBufferDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
	std::uint32_t sizeInBytes, BufferUsage bufferUsage)
	: deviceContext(deviceContextIn)
{
	try {
		nativeIndexBuffer = HardwareBufferHelper::CreateIndexBuffer(
			nativeDevice, nullptr, sizeInBytes, bufferUsage);
	}
	catch (std::exception const& exception) {
		throw exception;
	}
}
//-----------------------------------------------------------------------
IndexBufferDirect3D11::IndexBufferDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
	void const* vertices, std::uint32_t sizeInBytes, BufferUsage bufferUsage)
	: deviceContext(deviceContextIn)
{
	try {
		nativeIndexBuffer = HardwareBufferHelper::CreateIndexBuffer(
			nativeDevice, vertices, sizeInBytes, bufferUsage);
	}
	catch (std::exception const& exception) {
		throw exception;
	}
}
//-----------------------------------------------------------------------
void IndexBufferDirect3D11::SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t sizeInBytes)
{
	POMDOG_ASSERT(nativeIndexBuffer);
	POMDOG_ASSERT(deviceContext);
	HardwareBufferHelper::SetData(nativeIndexBuffer.Get(), deviceContext.Get(), offsetInBytes, source, sizeInBytes);
}
//-----------------------------------------------------------------------
ID3D11Buffer* IndexBufferDirect3D11::NativeBuffer() const
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.Get();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
