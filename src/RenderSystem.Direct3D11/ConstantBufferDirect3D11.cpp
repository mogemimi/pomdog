// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ConstantBufferDirect3D11.hpp"
#include "HardwareBufferHelper.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
//-----------------------------------------------------------------------
ConstantBufferDirect3D11::ConstantBufferDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn, std::uint32_t sizeInBytes)
	: deviceContext(deviceContextIn)
{
	try {
		nativeConstantBuffer = HardwareBufferHelper::CreateConstantBuffer(
			nativeDevice, nullptr, sizeInBytes, BufferUsage::Dynamic);
	}
	catch (std::exception const& exception) {
		throw exception;
	}
}
//-----------------------------------------------------------------------
void ConstantBufferDirect3D11::GetData(std::uint32_t sizeInBytes, void* result) const
{
	POMDOG_ASSERT(nativeConstantBuffer);
	POMDOG_ASSERT(deviceContext);
	HardwareBufferHelper::GetData(nativeConstantBuffer.Get(), deviceContext.Get(), 0, sizeInBytes, result);
}
//-----------------------------------------------------------------------
void ConstantBufferDirect3D11::SetData(std::uint32_t offsetInBytes,
	void const* source, std::uint32_t sizeInBytes)
{
	POMDOG_ASSERT(nativeConstantBuffer);
	POMDOG_ASSERT(deviceContext);
	HardwareBufferHelper::SetData(nativeConstantBuffer.Get(), deviceContext.Get(), offsetInBytes, source, sizeInBytes);
}
//-----------------------------------------------------------------------
ID3D11Buffer* ConstantBufferDirect3D11::NativeBuffer() const
{
	POMDOG_ASSERT(nativeConstantBuffer);
	return nativeConstantBuffer.Get();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
