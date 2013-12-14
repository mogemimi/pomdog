//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "VertexBufferGL4.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Utility/ScopeGuard.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
template<> struct TypesafeHelperGL4::OpenGLGetTraits<VertexBufferObjectGL4>
{
	constexpr static GLenum parameter_name = GL_ARRAY_BUFFER_BINDING;
};
//-----------------------------------------------------------------------
static GLenum ToVertexBufferUsage(BufferUsage bufferUsage)
{
	switch (bufferUsage) {
	case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
	//case BufferUsage::Dynamic: return GL_STREAM_DRAW;
	case BufferUsage::Immutable: return GL_STATIC_DRAW;
	}
#ifdef _MSC_VER
	return GL_STATIC_DRAW;
#endif
}
//-----------------------------------------------------------------------
VertexBufferGL4::VertexBufferGL4(void const* vertices, std::size_t vertexCount,
	VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage)
	: bufferObject(0U)
	, bufferObjectEnable(false)
{
	POMDOG_ASSERT(vertices != nullptr);
	POMDOG_ASSERT(vertexCount > 0);

	auto const oldBufferObject = TypesafeHelperGL4::Get<VertexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		glBindBuffer(GL_ARRAY_BUFFER, oldBufferObject.value);
	});

	// Generate vertex buffer
	glGenBuffers(1, &bufferObject);
	bufferObjectEnable = true;
	
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject.value);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif
	
	glBufferData(GL_ARRAY_BUFFER, vertexDeclaration.GetStrideBytes() * vertexCount,
		static_cast<GLvoid const*>(vertices),
		ToVertexBufferUsage(bufferUsage));

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
VertexBufferGL4::~VertexBufferGL4()
{
	if (bufferObjectEnable) {
		glDeleteBuffers(1, &bufferObject);
	}
}
//-----------------------------------------------------------------------
void VertexBufferGL4::SetData(void const* source, std::size_t vertexCount,
	VertexDeclaration const& vertexDeclaration)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(vertexCount > 0);

	auto const oldBufferObject = TypesafeHelperGL4::Get<VertexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		glBindBuffer(GL_ARRAY_BUFFER, oldBufferObject.value);
	});

	POMDOG_ASSERT(bufferObjectEnable);
	glBindBuffer(GL_ARRAY_BUFFER, oldBufferObject.value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(vertexDeclaration.GetStrideBytes() > 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDeclaration.GetStrideBytes() * vertexCount, source);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferSubData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
