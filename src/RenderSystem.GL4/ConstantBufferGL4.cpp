// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ConstantBufferGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<ConstantBufferObjectGL4> {
    constexpr static GLenum bufferObjectBinding = GL_UNIFORM_BUFFER_BINDING;
    constexpr static GLenum bufferObjectTarget = GL_UNIFORM_BUFFER;
};
//-----------------------------------------------------------------------
ConstantBufferGL4::ConstantBufferGL4(std::size_t byteWidth)
{
    // Generate constant buffer
    bufferObject = ([] {
        ConstantBufferObjectGL4 constantBuffer;
        glGenBuffers(1, constantBuffer.Data());
        return std::move(constantBuffer);
    })();

    auto const oldBuffer = TypesafeHelperGL4::Get<ConstantBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

    glBufferData(GL_UNIFORM_BUFFER, byteWidth, nullptr, GL_DYNAMIC_DRAW);
    POMDOG_CHECK_ERROR_GL4("glBufferData");
}
//-----------------------------------------------------------------------
ConstantBufferGL4::~ConstantBufferGL4()
{
    if (bufferObject) {
        glDeleteBuffers(1, bufferObject->Data());
    }
}
//-----------------------------------------------------------------------
void ConstantBufferGL4::GetData(std::size_t byteWidth, void* result) const
{
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(byteWidth > 0);

    auto const oldBuffer = TypesafeHelperGL4::Get<ConstantBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(DEBUG) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        POMDOG_ASSERT(byteWidth <= static_cast<std::size_t>(bufferSize));
    }
#endif

    glGetBufferSubData(GL_UNIFORM_BUFFER, 0, byteWidth, result);
    POMDOG_CHECK_ERROR_GL4("glGetBufferSubData");
}
//-----------------------------------------------------------------------
void ConstantBufferGL4::SetData(std::size_t offsetInBytes,
    void const* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(source != nullptr);

    auto const oldBuffer = TypesafeHelperGL4::Get<ConstantBufferObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindBuffer(oldBuffer); });

    POMDOG_ASSERT(bufferObject);
    TypesafeHelperGL4::BindBuffer(*bufferObject);
    POMDOG_CHECK_ERROR_GL4("glBindBuffer");

#if defined(DEBUG) && !defined(NDEBUG)
    {
        GLint bufferSize = 0;
        glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        POMDOG_ASSERT(sizeInBytes <= static_cast<std::size_t>(bufferSize));
    }
#endif

    POMDOG_ASSERT(sizeInBytes > 0);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetInBytes, sizeInBytes, source);
    POMDOG_CHECK_ERROR_GL4("glBufferSubData");
}
//-----------------------------------------------------------------------
void ConstantBufferGL4::Apply(int slotIndex)
{
    POMDOG_ASSERT(bufferObject);

#if defined(DEBUG) && !defined(NDEBUG)
    {
        static auto const maxUniformBufferBindings = ([] {
            GLint value = 0;
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);
            return value;
        })();

        POMDOG_ASSERT(slotIndex < maxUniformBufferBindings);
    }
#endif

    POMDOG_ASSERT(slotIndex >= 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, slotIndex, bufferObject->value);
}
//-----------------------------------------------------------------------
} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
