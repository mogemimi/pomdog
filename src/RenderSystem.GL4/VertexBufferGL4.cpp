//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "VertexBufferGL4.hpp"
#include <utility>
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
namespace {

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

}// unnamed namespace
//-----------------------------------------------------------------------
template<> struct TypesafeHelperGL4::OpenGLGetTraits<VertexBufferObjectGL4>
{
	constexpr static GLenum parameter_name = GL_ARRAY_BUFFER_BINDING;
};
//-----------------------------------------------------------------------
VertexBufferGL4::VertexBufferGL4(void const* vertices, std::size_t vertexCount,
	VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage)
{
	POMDOG_ASSERT(vertices != nullptr);
	POMDOG_ASSERT(vertexCount > 0);

	// Generate vertex buffer
	bufferObject = ([](){
		VertexBufferObjectGL4 vertexBuffer;
		glGenBuffers(1, vertexBuffer.data());
		return std::move(vertexBuffer);
	})();
	
	auto const oldBufferObject = TypesafeHelperGL4::Get<VertexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		glBindBuffer(GL_ARRAY_BUFFER, oldBufferObject.value);
	});
	
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject->value);
	
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
	if (bufferObject) {
		glDeleteBuffers(1, bufferObject->data());
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

	POMDOG_ASSERT(bufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject->value);

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
void VertexBufferGL4::BindBuffer()
{
	POMDOG_ASSERT(bufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject->value);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
