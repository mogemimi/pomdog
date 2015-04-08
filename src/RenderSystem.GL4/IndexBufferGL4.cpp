// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "IndexBufferGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
namespace {

static GLenum ToIndexBufferUsage(BufferUsage bufferUsage) noexcept
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

} // unnamed namespace
//-----------------------------------------------------------------------
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<IndexBufferObjectGL4> {
    constexpr static GLenum bufferObjectBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING;
    constexpr static GLenum bufferObjectTarget = GL_ELEMENT_ARRAY_BUFFER;
};
//-----------------------------------------------------------------------
IndexBufferGL4::IndexBufferGL4(std::size_t sizeInBytes, BufferUsage bufferUsage)
    : IndexBufferGL4(nullptr, sizeInBytes, bufferUsage)
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
}
//-----------------------------------------------------------------------
IndexBufferGL4::IndexBufferGL4(void const* indices,
    std::size_t sizeInBytes, BufferUsage bufferUsage)
{
    POMDOG_ASSERT(bufferUsage == BufferUsage::Immutable ? indices != nullptr: true);

    // Generate index buffer
    bufferObject = ([]{
        IndexBufferObjectGL4 indexBuffer;
        glGenBuffers(1, indexBuffer.Data());
        return std::move(indexBuffer);
    })();

    auto const oldBuffer = TypesafeHelperGL4::Get<IndexBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    POMDOG_ASSERT(sizeInBytes > 0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, indices,
        ToIndexBufferUsage(bufferUsage));
    POMDOG_CHECK_ERROR_GL4("glBufferData");
}
//-----------------------------------------------------------------------
IndexBufferGL4::~IndexBufferGL4()
{
    if (bufferObject) {
        glDeleteBuffers(1, bufferObject->Data());
    }
}
//-----------------------------------------------------------------------
void IndexBufferGL4::GetData(std::size_t sizeInBytes, void* result) const
{
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto const oldBuffer = TypesafeHelperGL4::Get<IndexBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(DEBUG) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        POMDOG_ASSERT(sizeInBytes <= static_cast<std::size_t>(bufferSize));
    }
#endif

    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeInBytes, result);
    POMDOG_CHECK_ERROR_GL4("glGetBufferSubData");
}
//-----------------------------------------------------------------------
void IndexBufferGL4::SetData(std::size_t offsetInBytes,
    void const* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(source != nullptr);

    auto const oldBuffer = TypesafeHelperGL4::Get<IndexBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    POMDOG_ASSERT(sizeInBytes > 0);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetInBytes,
        sizeInBytes, source);
    POMDOG_CHECK_ERROR_GL4("glBufferSubData");
}
//-----------------------------------------------------------------------
void IndexBufferGL4::BindBuffer()
{
    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");
}
//-----------------------------------------------------------------------
} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
