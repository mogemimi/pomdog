// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP
#define POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantBuffer {
public:
	NativeConstantBuffer() = default;
	NativeConstantBuffer(NativeConstantBuffer const&) = delete;
	NativeConstantBuffer & operator=(NativeConstantBuffer const&) = delete;

	virtual ~NativeConstantBuffer() = default;

	///@copydoc ConstantBuffer
	virtual void GetData(std::uint32_t byteWidth, void* result) const = 0;

	virtual void SetData(std::uint32_t offsetInBytes,
		void const* source, std::uint32_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP)
