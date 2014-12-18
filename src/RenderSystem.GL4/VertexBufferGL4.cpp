//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "VertexBufferGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <utility>

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
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<VertexBufferObjectGL4> {
	constexpr static GLenum bufferObjectBinding = GL_ARRAY_BUFFER_BINDING;
	constexpr static GLenum bufferObjectTarget = GL_ARRAY_BUFFER;
};
//-----------------------------------------------------------------------
VertexBufferGL4::VertexBufferGL4(std::uint32_t sizeInBytes, BufferUsage bufferUsage)
	: VertexBufferGL4(static_cast<void const*>(nullptr), sizeInBytes, bufferUsage)
{
	POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
}
//-----------------------------------------------------------------------
VertexBufferGL4::VertexBufferGL4(void const* vertices, std::uint32_t sizeInBytes,
	BufferUsage bufferUsage)
{
	POMDOG_ASSERT(bufferUsage == BufferUsage::Immutable ? vertices != nullptr: true);
	
	// Generate vertex buffer
	bufferObject = ([] {
		VertexBufferObjectGL4 vertexBuffer;
		glGenBuffers(1, vertexBuffer.Data());
		return std::move(vertexBuffer);
	})();
	
	auto const oldBufferObject = TypesafeHelperGL4::Get<VertexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		TypesafeHelperGL4::BindBuffer(oldBufferObject);
	});
	
	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif
	
	POMDOG_ASSERT(sizeInBytes > 0);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, vertices,
		ToVertexBufferUsage(bufferUsage));

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
VertexBufferGL4::~VertexBufferGL4()
{
	if (bufferObject) {
		glDeleteBuffers(1, bufferObject->Data());
	}
}
//-----------------------------------------------------------------------
void VertexBufferGL4::SetData(std::uint32_t offsetInBytes,
	void const* source, std::uint32_t sizeInBytes)
{
	POMDOG_ASSERT(source != nullptr);

	auto const oldBufferObject = TypesafeHelperGL4::Get<VertexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		TypesafeHelperGL4::BindBuffer(oldBufferObject);
	});

	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(sizeInBytes > 0);
	glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, sizeInBytes, source);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferSubData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void VertexBufferGL4::BindBuffer()
{
	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
