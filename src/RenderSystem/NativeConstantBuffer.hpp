//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeConstantBuffer {
public:
	NativeConstantBuffer() = default;
	NativeConstantBuffer(NativeConstantBuffer const&) = delete;
	NativeConstantBuffer & operator=(NativeConstantBuffer const&) = delete;

	virtual ~NativeConstantBuffer() = default;

	///@copydoc ConstantBuffer
	virtual void GetData(std::uint32_t byteWidth, void* result) const = 0;
	
	///@copydoc ConstantBuffer
	virtual void SetData(void const* data, std::uint32_t byteWidth) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP)
