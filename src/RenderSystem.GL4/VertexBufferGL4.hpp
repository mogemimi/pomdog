//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP
#define POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using VertexBufferObjectGL4 = Tagged<GLuint, VertexBuffer>;

class VertexBufferGL4 final: public NativeVertexBuffer {
public:
	VertexBufferGL4() = delete;
	~VertexBufferGL4() override;
	
	VertexBufferGL4(std::uint32_t sizeInBytes, BufferUsage bufferUsage);
		
	VertexBufferGL4(void const* vertices, std::uint32_t sizeInBytes, BufferUsage bufferUsage);

	///@copydoc NativeVertexBuffer
	void SetData(std::uint32_t offsetInBytes, void const* source,
		std::uint32_t sizeInBytes) override;
	
	void BindBuffer();
	
private:
	Optional<VertexBufferObjectGL4> bufferObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP)
