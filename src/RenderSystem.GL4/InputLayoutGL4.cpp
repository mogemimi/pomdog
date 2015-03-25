// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "InputLayoutGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "VertexBufferGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
namespace {

static ScalarTypeGL4 ToScalarType(GLenum attributeClass)
{
    switch (attributeClass) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
        return ScalarTypeGL4(GL_FLOAT);

    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
        return ScalarTypeGL4(GL_INT);

    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
        return ScalarTypeGL4(GL_UNSIGNED_INT);

    case GL_DOUBLE:
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3:
        return ScalarTypeGL4(GL_DOUBLE);

    case GL_BYTE:
        return ScalarTypeGL4(GL_BYTE);

    case GL_UNSIGNED_BYTE:
        return ScalarTypeGL4(GL_UNSIGNED_BYTE);

    case GL_SHORT:
        return ScalarTypeGL4(GL_SHORT);

    case GL_UNSIGNED_SHORT:
        return ScalarTypeGL4(GL_UNSIGNED_SHORT);
    }

    // Not supported:
    POMDOG_ASSERT(attributeClass != GL_HALF_FLOAT);
    POMDOG_ASSERT(attributeClass != GL_FIXED);

#ifdef DEBUG
    Log::Stream(LogLevel::Internal)
        << "Failed to find scalar type '"  << attributeClass << "'.";
#endif
    return ScalarTypeGL4(GL_FLOAT);
}
//-----------------------------------------------------------------------
static bool IsIntegerType(ScalarTypeGL4 const& scalarType)
{
    switch (scalarType.value) {
    case GL_FLOAT:
    case GL_DOUBLE:
        return false;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return true;
    }

    // Not supported:
    POMDOG_ASSERT(scalarType.value != GL_HALF_FLOAT);
    POMDOG_ASSERT(scalarType.value != GL_FIXED);

#ifdef DEBUG
    Log::Stream(LogLevel::Internal)
        << "Failed to find scalar type '"  << scalarType.value << "'.";
#endif
    return false;
}
//-----------------------------------------------------------------------
struct InputElementSize {
    ///@note
    /// float: 1
    /// Vector2: 1
    /// Vector3: 1
    /// Vector4: 1
    /// Matrix2x2: 2
    /// Matrix3x3: 3
    /// Matrix4x4: 4
    std::uint8_t SemanticIndex;

    ///@note
    /// float: 1
    /// Vector2: 2
    /// Vector3: 3
    /// Vector4: 4
    /// Matrix2x2: 2
    /// Matrix3x3: 3
    /// Matrix4x4: 4
    std::uint8_t ComponentCount;
};
//-----------------------------------------------------------------------
static InputElementSize ToInputElementSize(GLenum attributeClass)
{
    switch (attributeClass) {
    case GL_FLOAT:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_DOUBLE:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    // 'GL_UNSIGNED_INT_VEC' See also: http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveAttrib.xml
    //case GL_UNSIGNED_INT_VEC:
        return { 1, 1 };

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_DOUBLE_VEC2:
        return { 1, 2 };

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_DOUBLE_VEC3:
        return { 1, 3 };

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_DOUBLE_VEC4:
        return { 1, 4 };

    case GL_FLOAT_MAT2:
    case GL_DOUBLE_MAT2:
        return { 2, 2 };

    case GL_FLOAT_MAT3:
    case GL_DOUBLE_MAT3:
        return { 3, 3 };

    case GL_FLOAT_MAT4:
    case GL_DOUBLE_MAT4:
        return { 4, 4 };

    case GL_FLOAT_MAT2x3:
    case GL_DOUBLE_MAT2x3:
        return { 2, 3 };

    case GL_FLOAT_MAT3x2:
    case GL_DOUBLE_MAT3x2:
        return { 3, 2 };

    case GL_FLOAT_MAT2x4:
    case GL_DOUBLE_MAT2x4:
        return { 2, 4 };

    case GL_FLOAT_MAT4x2:
    case GL_DOUBLE_MAT4x2:
        return { 4, 2 };

    case GL_FLOAT_MAT3x4:
    case GL_DOUBLE_MAT3x4:
        return { 3, 4 };

    case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT4x3:
        return { 4, 3 };
    }

    // Not supported:
    POMDOG_ASSERT(attributeClass != GL_HALF_FLOAT);
    POMDOG_ASSERT(attributeClass != GL_FIXED);

#ifdef DEBUG
    Log::Stream(LogLevel::Critical)
        << "Failed to find class '" << attributeClass << "'.";
#endif // defined(DEBUG)
    return { 1, 1 };
}
//-----------------------------------------------------------------------
std::uint8_t ToByteWithFromScalarTypeGL4(ScalarTypeGL4 scalarType)
{
    switch (scalarType.value) {
    case GL_FLOAT: return sizeof(GLfloat);
    case GL_INT: return sizeof(GLint);
    case GL_UNSIGNED_INT: return sizeof(GLuint);
    case GL_DOUBLE: return sizeof(GLdouble);
    case GL_BYTE: return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
    case GL_SHORT: return sizeof(GLshort);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    }

    // Not supported:
    POMDOG_ASSERT(scalarType.value != GL_HALF_FLOAT);
    POMDOG_ASSERT(scalarType.value != GL_FIXED);

    #ifdef DEBUG
    Log::Stream(LogLevel::Internal)
        << "Failed to find scalar type '" << scalarType.value << "'.";
    #endif
    return sizeof(float);
}
//-----------------------------------------------------------------------
static std::vector<InputElementGL4> BuildAttributes(ShaderProgramGL4 const& shaderProgram)
{
    std::vector<InputElementGL4> attributes;

    GLint attributeCount = 0;
    GLint maxAttributeLength = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTES, &attributeCount);
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength);

    #ifdef DEBUG
    ErrorChecker::CheckError("glGetProgramiv", __FILE__, __LINE__);
    #endif

    POMDOG_ASSERT(attributeCount >= 0);
    POMDOG_ASSERT(maxAttributeLength > 0);

    for (GLint i = 0; i < attributeCount; ++i)
    {
        GLenum attributeClass = 0;
        GLint attributeVariableArraySize = 0;
        std::vector<GLchar> name(maxAttributeLength + 1, '\0');

        // Get attribute variable
        glGetActiveAttrib(shaderProgram.value, i, maxAttributeLength, nullptr,
            &attributeVariableArraySize, &attributeClass, name.data());

        // Get attribute location
        auto attributeLocation = glGetAttribLocation(shaderProgram.value, name.data());

        // Get scalar type
        auto const attributeScalarType = ToScalarType(attributeClass);
        auto const attributeSize = ToInputElementSize(attributeClass);

        POMDOG_ASSERT(attributeSize.ComponentCount >= 1 && attributeSize.ComponentCount <= 4);
        POMDOG_ASSERT(attributeSize.SemanticIndex >= 1 && attributeSize.SemanticIndex <= 4);

        for (std::uint16_t row = 0; row < attributeSize.SemanticIndex * attributeVariableArraySize; ++row)
        {
            InputElementGL4 attribute;
            attribute.StartSlot = attributeLocation;
            attribute.ScalarType = attributeScalarType;
            attribute.Components = static_cast<GLint>(attributeSize.ComponentCount);
            attribute.ByteOffset = 0; // Note: See CalculateByteOffset function.
            attribute.IsInteger = IsIntegerType(attribute.ScalarType);

            POMDOG_ASSERT(attribute.Components >= 1 && attribute.Components <= 4);
            attributes.push_back(attribute);

            ///@todo badcode
            // For matrix class in GLSL:
            attributeLocation += 1;

            //#ifdef DEBUG
            //Log::Stream(LogLevel::Internal)
            //    << "[GLSL] Attribute: StartSlot = " << static_cast<std::uint32_t>(attribute.StartSlot)
            //    << ", Name = '" << name.data() << "'.";
            //#endif // defined(DEBUG)
        }
    }

    std::sort(std::begin(attributes), std::end(attributes), [](InputElementGL4 const& a, InputElementGL4 const& b) {
        return a.StartSlot < b.StartSlot;
    });

    //#ifdef DEBUG
    //for (auto& attribute: attributes)
    //{
    //    Log::Stream(LogLevel::Internal)
    //        << "[GLSL] Attribute: ScalarType: '"
    //        << ([](ScalarTypeGL4 const& scalarType)->std::string{
    //            switch (scalarType.value)
    //            {
    //            case GL_FLOAT: return "GLfloat";
    //            case GL_INT: return "GLint";
    //            case GL_UNSIGNED_INT: return "GLuint";
    //            case GL_DOUBLE: return "GLdouble";
    //            case GL_BYTE: return "GLbyte";
    //            case GL_UNSIGNED_BYTE: return "GLubyte";
    //            case GL_SHORT: return "GLshort";
    //            case GL_UNSIGNED_SHORT: return "GLushort";
    //            }
    //            return "This type not supported";
    //        })(attribute.ScalarType)
    //        << "' StartSlot: '" << static_cast<std::uint32_t>(attribute.StartSlot)
    //        << "' Components: '" << static_cast<std::uint32_t>(attribute.Components)
    //        << "'";
    //};
    //#endif // defined(DEBUG)

    return std::move(attributes);
}
//-----------------------------------------------------------------------
static void EnableAttributes(std::vector<InputElementGL4> const& attributes)
{
    for (auto& attribute: attributes)
    {
        glEnableVertexAttribArray(attribute.StartSlot);

        #ifdef DEBUG
        ErrorChecker::CheckError("glEnableVertexAttribArray", __FILE__, __LINE__);
        #endif
    }
}
//-----------------------------------------------------------------------
static std::uint32_t CalculateByteOffset(std::uint32_t const offsetBytes, InputElementGL4 & attribute)
{
    attribute.ByteOffset = offsetBytes;

    std::uint32_t const byteWidth = ToByteWithFromScalarTypeGL4(attribute.ScalarType) * attribute.Components;
    POMDOG_ASSERT(byteWidth > 0);

    return offsetBytes + byteWidth;
}
//-----------------------------------------------------------------------
static std::vector<InputBindingGL4> BuildInputBindings(std::vector<InputElementGL4> const& attributes)
{
    POMDOG_ASSERT(!attributes.empty());

    std::vector<InputBindingGL4> result(1);
    POMDOG_ASSERT(!result.empty());

    InputBindingGL4 & inputBinding = result.front();
    inputBinding.InstanceStepRate = 0;

    std::uint32_t offsetBytes = 0;
    for (auto const& attribute: attributes)
    {
        inputBinding.InputElements.push_back(attribute);
        offsetBytes = CalculateByteOffset(offsetBytes, inputBinding.InputElements.back());
    }
    return std::move(result);
}
//-----------------------------------------------------------------------
static std::vector<InputBindingGL4> BuildInputBindings(
    std::vector<VertexBufferBinding> const& vertexBindings,
    std::vector<InputElementGL4> const& attributes)
{
    POMDOG_ASSERT(!vertexBindings.empty());
    POMDOG_ASSERT(!attributes.empty());

    std::vector<InputBindingGL4> result;

    auto attributeIter = std::begin(attributes);

    for (auto & binding: vertexBindings)
    {
        auto const strideBytes = binding.Declaration.StrideBytes();

        std::uint32_t offsetBytes = 0;
        POMDOG_ASSERT(attributeIter != std::end(attributes));

        InputBindingGL4 inputBinding;
        inputBinding.InstanceStepRate = binding.InstanceStepRate;

        attributeIter = std::find_if(attributeIter, std::end(attributes), [&](InputElementGL4 const& attribute)
        {
            inputBinding.InputElements.push_back(attribute);
            offsetBytes = CalculateByteOffset(offsetBytes, inputBinding.InputElements.back());
            return offsetBytes >= strideBytes;
        });

        POMDOG_ASSERT(offsetBytes == strideBytes);

        result.emplace_back(std::move(inputBinding));

        if (attributeIter != std::end(attributes)) {
            ++attributeIter;
        }
    }
    POMDOG_ASSERT(attributeIter == std::end(attributes));

    return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T> static
GLubyte const* ComputeBufferOffset(T const offsetBytes)
{
    static_assert(std::is_unsigned<T>::value, "T is unsigned type.");
    return reinterpret_cast<GLubyte const*>(0) + offsetBytes;
}
//-----------------------------------------------------------------------
static void ApplyInputBindings(std::vector<InputBindingGL4> const& inputBindings,
    std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers)
{
    POMDOG_ASSERT(!inputBindings.empty());
    POMDOG_ASSERT(inputBindings.size() == vertexBuffers.size());

    auto vertexBufferIter = std::begin(vertexBuffers);

    for (auto & binding: inputBindings)
    {
        POMDOG_ASSERT(vertexBufferIter != std::end(vertexBuffers));
        auto & vertexBuffer = (*vertexBufferIter);
        auto nativeVertexBuffer = dynamic_cast<VertexBufferGL4*>(vertexBuffer->NativeVertexBuffer());

        POMDOG_ASSERT(nativeVertexBuffer);
        // NOTE:
        // The following code is the same as `glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer)`.
        nativeVertexBuffer->BindBuffer();

        POMDOG_ASSERT(vertexBuffer->StrideBytes() > 0);
        POMDOG_ASSERT(vertexBuffer->StrideBytes() <= static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()));

        const auto strideBytes = static_cast<GLsizei>(vertexBuffer->StrideBytes());

        for (auto & attribute: binding.InputElements)
        {
            if (attribute.IsInteger)
            {
                glVertexAttribIPointer(
                    attribute.StartSlot,
                    attribute.Components,
                    attribute.ScalarType.value,
                    strideBytes,
                    ComputeBufferOffset(attribute.ByteOffset));

                #ifdef DEBUG
                ErrorChecker::CheckError("glVertexAttribIPointer", __FILE__, __LINE__);
                #endif
            }
            else
            {
                glVertexAttribPointer(
                    attribute.StartSlot,
                    attribute.Components,
                    attribute.ScalarType.value,
                    GL_FALSE,
                    strideBytes,
                    ComputeBufferOffset(attribute.ByteOffset));

                #ifdef DEBUG
                ErrorChecker::CheckError("glVertexAttribPointer", __FILE__, __LINE__);
                #endif
            }

            glVertexAttribDivisor(attribute.StartSlot, binding.InstanceStepRate);
            #ifdef DEBUG
            ErrorChecker::CheckError("glVertexAttribDivisor", __FILE__, __LINE__);
            #endif
        }

        ++vertexBufferIter;
    }
    POMDOG_ASSERT(vertexBufferIter == std::end(vertexBuffers));
}
//-----------------------------------------------------------------------
}// unnamed namespace
//-----------------------------------------------------------------------
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<VertexArrayGL4> {
    constexpr static GLenum bufferObjectBinding = GL_VERTEX_ARRAY_BINDING;
};
//-----------------------------------------------------------------------
InputLayoutGL4::InputLayoutGL4(ShaderProgramGL4 const& shaderProgram)
    : InputLayoutGL4(shaderProgram, {})
{}
//-----------------------------------------------------------------------
InputLayoutGL4::InputLayoutGL4(ShaderProgramGL4 const& shaderProgram,
    std::vector<VertexBufferBinding> const& vertexBinding)
{
    // Build vertex array object
    inputLayout = ([] {
        VertexArrayGL4 vertexArray;
        glGenVertexArrays(1, vertexArray.Data());
        return std::move(vertexArray);
    })();

    auto const oldInputLayout = TypesafeHelperGL4::Get<VertexArrayGL4>();
    ScopeGuard scope([&oldInputLayout]{
        glBindVertexArray(oldInputLayout.value);
    });

    glBindVertexArray(inputLayout->value);

    #ifdef DEBUG
    ErrorChecker::CheckError("glBindVertexArray", __FILE__, __LINE__);
    #endif

    auto const attributes = BuildAttributes(shaderProgram);

    if (vertexBinding.empty()) {
        inputBindings = BuildInputBindings(attributes);
    }
    else {
        inputBindings = BuildInputBindings(vertexBinding, attributes);
    }

    for (auto& bindings: inputBindings) {
        EnableAttributes(bindings.InputElements);
    }
}
//-----------------------------------------------------------------------
InputLayoutGL4::~InputLayoutGL4()
{
    if (inputLayout) {
        glDeleteVertexArrays(1, inputLayout->Data());
    }
}
//-----------------------------------------------------------------------
void InputLayoutGL4::Apply(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers)
{
    POMDOG_ASSERT(inputLayout);
    glBindVertexArray(inputLayout->value);

    #ifdef DEBUG
    ErrorChecker::CheckError("glBindVertexArray", __FILE__, __LINE__);
    #endif

    //for (auto& bindings: inputBindings) {
    //    EnableAttributes(bindings.InputElements);
    //}

    ApplyInputBindings(inputBindings, vertexBuffers);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
