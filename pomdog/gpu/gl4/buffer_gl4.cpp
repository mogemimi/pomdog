// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

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

[[nodiscard]] GLenum
toBufferTarget(BufferBindFlags bindFlags) noexcept
{
    if (hasFlag(bindFlags, BufferBindFlags::ConstantBuffer)) {
        return GL_UNIFORM_BUFFER;
    }
    if (hasFlag(bindFlags, BufferBindFlags::IndexBuffer)) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

[[nodiscard]] GLenum
toBufferBinding(GLenum bufferTarget) noexcept
{
    switch (bufferTarget) {
    case GL_UNIFORM_BUFFER:
        return GL_UNIFORM_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_ARRAY_BUFFER:
        return GL_ARRAY_BUFFER_BINDING;
    default:
        break;
    }
    return GL_ARRAY_BUFFER_BINDING;
}

} // namespace

BufferGL4::~BufferGL4()
{
    if (bufferObject_) {
        glDeleteBuffers(1, &(*bufferObject_));
        POMDOG_CHECK_ERROR_GL4("glDeleteBuffers");
    }
}

void BufferGL4::getData(
    [[maybe_unused]] u32 offsetInBytes,
    [[maybe_unused]] std::span<u8> destination) const
{
    POMDOG_ASSERT(!destination.empty());
    POMDOG_ASSERT(destination.data() != nullptr);
    POMDOG_ASSERT(bufferObject_);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: glGetBufferSubData is not available in WebGL 2.0 (OpenGL ES 3.0).
#else
    GLint oldBuffer = 0;
    glGetIntegerv(bufferBinding_, &oldBuffer);
    ScopeGuard scope([&] { glBindBuffer(bufferTarget_, static_cast<GLuint>(oldBuffer)); });

    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(bufferTarget_, GL_BUFFER_SIZE, &bufferSize);
        POMDOG_ASSERT(bufferSize > 0);
        POMDOG_ASSERT((offsetInBytes + destination.size()) <= static_cast<u32>(bufferSize));
    }
#endif

    glGetBufferSubData(bufferTarget_, offsetInBytes, destination.size(), destination.data());
    POMDOG_CHECK_ERROR_GL4("glGetBufferSubData");
#endif
}

void BufferGL4::setData(
    u32 offsetInBytes,
    std::span<const u8> source)
{
    POMDOG_ASSERT(!source.empty());
    POMDOG_ASSERT(source.data() != nullptr);
    POMDOG_ASSERT(bufferObject_);

    GLint oldBuffer = 0;
    glGetIntegerv(bufferBinding_, &oldBuffer);
    ScopeGuard scope([&] { glBindBuffer(bufferTarget_, static_cast<GLuint>(oldBuffer)); });

    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(bufferTarget_, GL_BUFFER_SIZE, &bufferSize);
        POMDOG_ASSERT(bufferSize > 0);
        POMDOG_ASSERT((offsetInBytes + source.size()) <= static_cast<u32>(bufferSize));
    }
#endif

    POMDOG_ASSERT(!source.empty());
    glBufferSubData(bufferTarget_, offsetInBytes, source.size(), source.data());
    POMDOG_CHECK_ERROR_GL4("glBufferSubData");
}

void BufferGL4::bindBuffer()
{
    POMDOG_ASSERT(bufferObject_);
    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");
}

GLuint
BufferGL4::getBuffer() const noexcept
{
    POMDOG_ASSERT(bufferObject_);
    return *bufferObject_;
}

std::unique_ptr<Error>
BufferGL4::initialize(
    const BufferDesc& desc,
    std::span<const u8> sourceData) noexcept
{
    if (!sourceData.empty()) {
        if (sourceData.size() < desc.sizeInBytes) {
            return errors::make("sourceData size must be >= desc.sizeInBytes");
        }
    }

    memoryUsage_ = desc.memoryUsage;
    bufferTarget_ = toBufferTarget(desc.bindFlags);
    bufferBinding_ = toBufferBinding(bufferTarget_);

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    bufferObject_ = buffer;
    POMDOG_CHECK_ERROR_GL4("glGenBuffers");

    GLint oldBuffer = 0;
    glGetIntegerv(bufferBinding_, &oldBuffer);
    ScopeGuard scope([&] { glBindBuffer(bufferTarget_, static_cast<GLuint>(oldBuffer)); });

    POMDOG_ASSERT(bufferObject_);
    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    POMDOG_ASSERT(desc.sizeInBytes > 0);
    glBufferData(
        bufferTarget_,
        desc.sizeInBytes,
        sourceData.empty() ? nullptr : sourceData.data(),
        toBufferUsage(desc.memoryUsage));
    POMDOG_CHECK_ERROR_GL4("glBufferData");

    return nullptr;
}

std::span<u8> BufferGL4::map(u32 offsetInBytes, u32 sizeInBytes)
{
    POMDOG_ASSERT(bufferObject_);
    POMDOG_ASSERT(memoryUsage_ != MemoryUsage::GpuOnly);

    GLint oldBuffer = 0;
    glGetIntegerv(bufferBinding_, &oldBuffer);
    ScopeGuard scope([&] { glBindBuffer(bufferTarget_, static_cast<GLuint>(oldBuffer)); });

    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    GLbitfield access = 0;
    if (memoryUsage_ == MemoryUsage::CpuToGpu) {
        access = GL_MAP_WRITE_BIT;
    }
    else if (memoryUsage_ == MemoryUsage::GpuToCpu) {
        access = GL_MAP_READ_BIT;
    }

    void* ptr = glMapBufferRange(
        bufferTarget_,
        offsetInBytes,
        sizeInBytes,
        access);
    POMDOG_CHECK_ERROR_GL4("glMapBufferRange");

    if (ptr == nullptr) {
        return {};
    }
    return std::span<u8>{static_cast<u8*>(ptr), sizeInBytes};
}

void BufferGL4::unmap()
{
    POMDOG_ASSERT(bufferObject_);

    GLint oldBuffer = 0;
    glGetIntegerv(bufferBinding_, &oldBuffer);
    ScopeGuard scope([&] { glBindBuffer(bufferTarget_, static_cast<GLuint>(oldBuffer)); });

    glBindBuffer(bufferTarget_, *bufferObject_);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    glUnmapBuffer(bufferTarget_);
    POMDOG_CHECK_ERROR_GL4("glUnmapBuffer");
}

} // namespace pomdog::gpu::detail::gl4
