//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "IndexBufferGL4.hpp"
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/IndexElementSize.hpp>
#include "../Utility/ScopeGuard.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static GLenum ToIndexBufferUsage(BufferUsage bufferUsage)
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
struct TypesafeHelperGL4::OpenGLGetTraits<IndexBufferObjectGL4> {
	constexpr static GLenum bufferObjectBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING;
	constexpr static GLenum bufferObjectTarget = GL_ELEMENT_ARRAY_BUFFER;
};
//-----------------------------------------------------------------------
IndexBufferGL4::IndexBufferGL4(void const* indices,
	std::uint32_t sizeInBytes, BufferUsage bufferUsage)
{
	// Generate index buffer
	bufferObject = ([]{
		IndexBufferObjectGL4 indexBuffer;
		glGenBuffers(1, indexBuffer.Data());
		return std::move(indexBuffer);
	})();
	
	auto const oldBufferObject = TypesafeHelperGL4::Get<IndexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		TypesafeHelperGL4::BindBuffer(oldBufferObject);
	});

	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(sizeInBytes > 0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes,
		static_cast<GLvoid const*>(indices),
		ToIndexBufferUsage(bufferUsage));

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
IndexBufferGL4::~IndexBufferGL4()
{
	if (bufferObject) {
		glDeleteBuffers(1, bufferObject->Data());
	}
}
//-----------------------------------------------------------------------
void IndexBufferGL4::SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t sizeInBytes)
{
	POMDOG_ASSERT(source != nullptr);

	auto const oldBufferObject = TypesafeHelperGL4::Get<IndexBufferObjectGL4>();
	ScopeGuard scope([&oldBufferObject]{
		TypesafeHelperGL4::BindBuffer(oldBufferObject);
	});

	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(sizeInBytes > 0);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetInBytes,
		sizeInBytes, source);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBufferSubData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void IndexBufferGL4::BindBuffer()
{
	POMDOG_ASSERT(bufferObject);
	TypesafeHelperGL4::BindBuffer(*bufferObject);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBindBuffer", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
