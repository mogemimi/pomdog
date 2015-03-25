// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP
#define POMDOG_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeConstantBuffer.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

struct ConstantBufferObjectTag {};

}// namespace Tags

using ConstantBufferObjectGL4 = Tagged<GLuint, Tags::ConstantBufferObjectTag>;

class ConstantBufferGL4 final: public NativeConstantBuffer {
public:
	ConstantBufferGL4() = delete;

	explicit ConstantBufferGL4(std::uint32_t sizeInBytes);

	~ConstantBufferGL4();

	///@copydoc NativeConstantBuffer
	void GetData(std::uint32_t byteWidth, void* result) const override;

	///@copydoc NativeConstantBuffer
	void SetData(std::uint32_t offsetInBytes,
		void const* source, std::uint32_t sizeInBytes) override;

	void Apply(std::uint32_t slotIndex);

private:
	Optional<ConstantBufferObjectGL4> bufferObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP)
