//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "VertexBufferDirect3D11.hpp"
#include "HardwareBufferHelper.hpp"
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
//-----------------------------------------------------------------------
VertexBufferDirect3D11::VertexBufferDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
	std::uint32_t sizeInBytes, BufferUsage bufferUsage)
	: deviceContext(deviceContextIn)
{
	try {
		nativeVertexBuffer = HardwareBufferHelper::CreateVertexBuffer(
			nativeDevice, nullptr, sizeInBytes, bufferUsage);
	}
	catch (std::exception const& exception) {
		throw exception;
	}
}
//-----------------------------------------------------------------------
VertexBufferDirect3D11::VertexBufferDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
	void const* vertices, std::uint32_t sizeInBytes, BufferUsage bufferUsage)
	: deviceContext(deviceContextIn)
{
	try {
		nativeVertexBuffer = HardwareBufferHelper::CreateVertexBuffer(
			nativeDevice, vertices, sizeInBytes, bufferUsage);
	}
	catch (std::exception const& exception) {
		throw exception;
	}
}
//-----------------------------------------------------------------------
void VertexBufferDirect3D11::SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t sizeInBytes)
{
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(deviceContext);
	HardwareBufferHelper::SetData(nativeVertexBuffer.Get(), deviceContext.Get(), offsetInBytes, source, sizeInBytes);
}
//-----------------------------------------------------------------------
ID3D11Buffer* VertexBufferDirect3D11::NativeBuffer() const
{
	POMDOG_ASSERT(nativeVertexBuffer);
	return nativeVertexBuffer.Get();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
