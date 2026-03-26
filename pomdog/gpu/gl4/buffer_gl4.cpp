// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
toBufferUsage(MemoryUsage memoryUsage) noexcept
{
    switch (memoryUsage) {
    case MemoryUsage::GpuOnly:
        return GL_STATIC_DRAW;
    case MemoryUsage::CpuToGpu:
        return GL_DYNAMIC_DRAW;
    case MemoryUsage::GpuToCpu:
        return GL_STREAM_READ;
    }
    POMDOG_UNREACHABLE("Unsupported memory usage");
}

template <class Tag>
struct BufferTraits;

template <>
struct BufferTraits<ConstantBuffer> final {
    constexpr static GLenum Buffer = GL_UNIFORM_BUFFER;
};

template <>
struct BufferTraits<IndexBuffer> final {
    constexpr static GLenum Buffer = GL_ELEMENT_ARRAY_BUFFER;
};

template <>
struct BufferTraits<VertexBuffer> final {
    constexpr static GLenum Buffer = GL_ARRAY_BUFFER;
};

} // namespace

template <>
struct TypesafeHelperGL4::Traits<BufferObjectGL4<ConstantBuffer>> final {
    constexpr static GLenum BufferBinding = GL_UNIFORM_BUFFER_BINDING;
    constexpr static GLenum BufferTarget = BufferTraits<ConstantBuffer>::Buffer;
};

template <>
struct TypesafeHelperGL4::Traits<BufferObjectGL4<IndexBuffer>> final {
    constexpr static GLenum BufferBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING;
    constexpr static GLenum BufferTarget = BufferTraits<IndexBuffer>::Buffer;
};

template <>
struct TypesafeHelperGL4::Traits<BufferObjectGL4<VertexBuffer>> final {
    constexpr static GLenum BufferBinding = GL_ARRAY_BUFFER_BINDING;
    constexpr static GLenum BufferTarget = BufferTraits<VertexBuffer>::Buffer;
};

template <class Tag>
BufferGL4<Tag>::~BufferGL4()
{
    if (bufferObject_) {
        glDeleteBuffers(1, bufferObject_->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteBuffers");
    }
}

template <class Tag>
void BufferGL4<Tag>::getData(
    u32 offsetInBytes,
    void* destination,
    u32 sizeInBytes) const
{
    POMDOG_ASSERT(destination != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto const oldBuffer = TypesafeHelperGL4::Get<BufferObject>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject_);
    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(
            BufferTraits<Tag>::Buffer,
            GL_BUFFER_SIZE,
            &bufferSize);
        POMDOG_ASSERT(bufferSize > 0);
        POMDOG_ASSERT((offsetInBytes + sizeInBytes) <= static_cast<u32>(bufferSize));
    }
#endif

    glGetBufferSubData(
        BufferTraits<Tag>::Buffer,
        offsetInBytes,
        sizeInBytes,
        destination);
    POMDOG_CHECK_ERROR_GL4("glGetBufferSubData");
}

template <class Tag>
void BufferGL4<Tag>::setData(
    u32 offsetInBytes,
    const void* source,
    u32 sizeInBytes)
{
    POMDOG_ASSERT(source != nullptr);

    auto const oldBuffer = TypesafeHelperGL4::Get<BufferObject>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject_);
    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(
            BufferTraits<Tag>::Buffer,
            GL_BUFFER_SIZE,
            &bufferSize);
        POMDOG_ASSERT(bufferSize > 0);
        POMDOG_ASSERT((offsetInBytes + sizeInBytes) <= static_cast<u32>(bufferSize));
    }
#endif

    POMDOG_ASSERT(sizeInBytes > 0);
    glBufferSubData(
        BufferTraits<Tag>::Buffer,
        offsetInBytes,
        sizeInBytes,
        source);
    POMDOG_CHECK_ERROR_GL4("glBufferSubData");
}

template <class Tag>
void BufferGL4<Tag>::bindBuffer()
{
    POMDOG_ASSERT(bufferObject_);
    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");
}

template <class Tag>
GLuint
BufferGL4<Tag>::getBuffer() const noexcept
{
    POMDOG_ASSERT(bufferObject_);
    return bufferObject_->value;
}

template <class Tag>
std::unique_ptr<Error>
BufferGL4<Tag>::initialize(
    const BufferDesc& desc,
    std::span<const u8> sourceData) noexcept
{
    if (!sourceData.empty()) {
        if (sourceData.size() < desc.sizeInBytes) {
            return errors::make("sourceData size must be >= desc.sizeInBytes");
        }
    }

    memoryUsage_ = desc.memoryUsage;

    bufferObject_ = ([] {
        BufferObject buffer;
        glGenBuffers(1, buffer.Data());
        return buffer;
    })();
    POMDOG_CHECK_ERROR_GL4("glGenBuffers");

    auto const oldBuffer = TypesafeHelperGL4::Get<BufferObject>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject_);
    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    POMDOG_ASSERT(desc.sizeInBytes > 0);
    glBufferData(
        BufferTraits<Tag>::Buffer,
        desc.sizeInBytes,
        sourceData.empty() ? nullptr : sourceData.data(),
        toBufferUsage(desc.memoryUsage));
    POMDOG_CHECK_ERROR_GL4("glBufferData");

    return nullptr;
}

template <class Tag>
std::span<u8> BufferGL4<Tag>::map(u32 offsetInBytes, u32 sizeInBytes)
{
    POMDOG_ASSERT(bufferObject_);
    POMDOG_ASSERT(memoryUsage_ != MemoryUsage::GpuOnly);

    auto const oldBuffer = TypesafeHelperGL4::Get<BufferObject>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    GLbitfield access = 0;
    if (memoryUsage_ == MemoryUsage::CpuToGpu) {
        access = GL_MAP_WRITE_BIT;
    }
    else if (memoryUsage_ == MemoryUsage::GpuToCpu) {
        access = GL_MAP_READ_BIT;
    }

    void* ptr = glMapBufferRange(
        BufferTraits<Tag>::Buffer,
        offsetInBytes,
        sizeInBytes,
        access);
    POMDOG_CHECK_ERROR_GL4("glMapBufferRange");

    if (ptr == nullptr) {
        return {};
    }
    return std::span<u8>{static_cast<u8*>(ptr), sizeInBytes};
}

template <class Tag>
void BufferGL4<Tag>::unmap()
{
    POMDOG_ASSERT(bufferObject_);

    auto const oldBuffer = TypesafeHelperGL4::Get<BufferObject>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    TypesafeHelperGL4::BindBuffer(*bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    glUnmapBuffer(BufferTraits<Tag>::Buffer);
    POMDOG_CHECK_ERROR_GL4("glUnmapBuffer");
}

// explicit instantiations
template class BufferGL4<ConstantBuffer>;
template class BufferGL4<IndexBuffer>;
template class BufferGL4<VertexBuffer>;

} // namespace pomdog::gpu::detail::gl4
