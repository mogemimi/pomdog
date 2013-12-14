//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP
#define POMDOG_SRC_GL4_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using VertexBufferObjectGL4 = Tagged<GLuint, VertexBuffer>;

class VertexBufferGL4 final: public NativeVertexBuffer
{
public:
	VertexBufferGL4() = delete;
	~VertexBufferGL4() override;
	
	VertexBufferGL4(void const* vertices, std::size_t vertexCount,
		VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage);

	///@copydoc NativeVertexBuffer
	void SetData(void const* source, std::size_t vertexCount,
		VertexDeclaration const& vertexDeclaration) override;
	
private:
	// TODO: std::optional<VertexBufferObjectGL4> using C++14
	VertexBufferObjectGL4 bufferObject;
	bool bufferObjectEnable;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_VERTEXBUFFERGL4_A0F173BB_E5E5_4A9B_81E8_4AC8BBA92F01_HPP)
