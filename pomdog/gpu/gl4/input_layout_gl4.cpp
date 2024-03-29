// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/input_layout_descriptor.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/logging/log.h"
#include "pomdog/logging/log_level.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::ScopeGuard;

namespace pomdog::gpu::detail::gl4 {
namespace {

ScalarTypeGL4 ToScalarType(GLenum attributeClass)
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

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::Internal("failed to find scalar type " + std::to_string(attributeClass));
#endif
    return ScalarTypeGL4(GL_FLOAT);
}

bool IsIntegerType(const ScalarTypeGL4& scalarType)
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

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::Internal("failed to find scalar type " + std::to_string(scalarType.value));
#endif
    return false;
}

struct InputElementSize final {
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
    std::int8_t ComponentCount;
};

InputElementSize ToInputElementSize(GLenum attributeClass)
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
        return {1, 1};

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_DOUBLE_VEC2:
        return {1, 2};

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_DOUBLE_VEC3:
        return {1, 3};

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_DOUBLE_VEC4:
        return {1, 4};

    case GL_FLOAT_MAT2:
    case GL_DOUBLE_MAT2:
        return {2, 2};

    case GL_FLOAT_MAT3:
    case GL_DOUBLE_MAT3:
        return {3, 3};

    case GL_FLOAT_MAT4:
    case GL_DOUBLE_MAT4:
        return {4, 4};

    case GL_FLOAT_MAT2x3:
    case GL_DOUBLE_MAT2x3:
        return {2, 3};

    case GL_FLOAT_MAT3x2:
    case GL_DOUBLE_MAT3x2:
        return {3, 2};

    case GL_FLOAT_MAT2x4:
    case GL_DOUBLE_MAT2x4:
        return {2, 4};

    case GL_FLOAT_MAT4x2:
    case GL_DOUBLE_MAT4x2:
        return {4, 2};

    case GL_FLOAT_MAT3x4:
    case GL_DOUBLE_MAT3x4:
        return {3, 4};

    case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT4x3:
        return {4, 3};
    }

    // Not supported:
    POMDOG_ASSERT(attributeClass != GL_HALF_FLOAT);
    POMDOG_ASSERT(attributeClass != GL_FIXED);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::Internal("failed to find class " + std::to_string(attributeClass));
#endif
    return {1, 1};
}

std::uint8_t ToByteWithFromScalarTypeGL4(ScalarTypeGL4 scalarType)
{
    switch (scalarType.value) {
    case GL_FLOAT:
        return sizeof(GLfloat);
    case GL_INT:
        return sizeof(GLint);
    case GL_UNSIGNED_INT:
        return sizeof(GLuint);
    case GL_DOUBLE:
        return sizeof(GLdouble);
    case GL_BYTE:
        return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    case GL_SHORT:
        return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
    }

    // Not supported:
    POMDOG_ASSERT(scalarType.value != GL_HALF_FLOAT);
    POMDOG_ASSERT(scalarType.value != GL_FIXED);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::Internal("failed to find scalar type " + std::to_string(scalarType.value));
#endif
    return sizeof(float);
}

std::vector<InputElementGL4> BuildAttributes(const ShaderProgramGL4& shaderProgram)
{
    std::vector<InputElementGL4> attributes;

    GLint attributeCount = 0;
    GLint maxAttributeLength = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTES, &attributeCount);
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    POMDOG_ASSERT(attributeCount >= 0);
    POMDOG_ASSERT(maxAttributeLength > 0);

    for (GLint i = 0; i < attributeCount; ++i) {
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

        for (std::uint16_t row = 0; row < attributeSize.SemanticIndex * attributeVariableArraySize; ++row) {
            InputElementGL4 attribute;
            attribute.attributeLocation = attributeLocation;
            attribute.scalarType = attributeScalarType;
            attribute.components = attributeSize.ComponentCount;
            attribute.isInteger = IsIntegerType(attributeScalarType);
            attribute.byteOffset = 0; // NOTE: See CalculateByteOffset function.
            attribute.inputSlot = 0;
            attribute.instanceStepRate = 0;

            POMDOG_ASSERT(attribute.components >= 1 && attribute.components <= 4);
            attributes.push_back(attribute);

            // FIXME: badcode
            // For matrix class in GLSL:
            attributeLocation += 1;
        }
    }

    std::sort(std::begin(attributes), std::end(attributes), [](const InputElementGL4& a, const InputElementGL4& b) {
        return a.attributeLocation < b.attributeLocation;
    });

    return attributes;
}

void EnableAttributes(const std::vector<InputElementGL4>& inputElements)
{
    for (auto& inputElement : inputElements) {
        glEnableVertexAttribArray(inputElement.attributeLocation);
        POMDOG_CHECK_ERROR_GL4("glEnableVertexAttribArray");
    }
}

std::uint32_t CalculateByteOffset(const ScalarTypeGL4& scalarType, std::int8_t components)
{
    POMDOG_ASSERT(components >= 1);
    POMDOG_ASSERT(components <= 4);

    return ToByteWithFromScalarTypeGL4(scalarType) * static_cast<std::uint32_t>(components);
}

std::vector<InputElementGL4>
BuildInputElements(std::vector<InputElementGL4>&& inputElements)
{
    POMDOG_ASSERT(!inputElements.empty());

    std::uint32_t offsetBytes = 0;
    for (auto& inputElement : inputElements) {
        inputElement.byteOffset = offsetBytes;
        inputElement.instanceStepRate = 0;
        inputElement.inputSlot = 0;
        offsetBytes += CalculateByteOffset(inputElement.scalarType, inputElement.components);
        POMDOG_ASSERT(offsetBytes > 0);
    }

    return std::move(inputElements);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
ScalarTypeGL4 ToScalarTypeGL4(InputElementFormat format)
{
    switch (format) {
    case InputElementFormat::Byte4:
        return {GL_BYTE};
    case InputElementFormat::Float:
    case InputElementFormat::Float2:
    case InputElementFormat::Float3:
    case InputElementFormat::Float4:
        return {GL_FLOAT};
    case InputElementFormat::HalfFloat2:
    case InputElementFormat::HalfFloat4:
        return {GL_HALF_FLOAT};
    case InputElementFormat::Int4:
        return {GL_INT};
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

std::int8_t ToComponentsGL4(InputElementFormat format)
{
    switch (format) {
    case InputElementFormat::Float:
        return 1;
    case InputElementFormat::Float2:
    case InputElementFormat::HalfFloat2:
        return 2;
    case InputElementFormat::Float3:
        return 3;
    case InputElementFormat::Byte4:
    case InputElementFormat::Float4:
    case InputElementFormat::HalfFloat4:
    case InputElementFormat::Int4:
        return 4;
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

GLuint GetMaxAttributeCount()
{
    static const GLuint maxAttributeCount = ([] {
        GLint maxCount = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxCount);
        return maxCount;
    })();
    return maxAttributeCount;
}
#endif

std::vector<InputElementGL4> BuildInputElements(
    const InputLayoutDescriptor& descriptor,
    std::vector<InputElementGL4>&& attributes)
{
    POMDOG_ASSERT(!descriptor.inputElements.empty());
    POMDOG_ASSERT(!attributes.empty());

    auto sortedElements = descriptor.inputElements;
    std::sort(std::begin(sortedElements), std::end(sortedElements),
        [](const InputElement& a, const InputElement& b) {
            if (a.inputSlot == b.inputSlot) {
                return a.byteOffset < b.byteOffset;
            }
            return a.inputSlot < b.inputSlot;
        });

    POMDOG_ASSERT(!sortedElements.empty());

    std::vector<InputElementGL4> inputElements;
    inputElements.reserve(sortedElements.size());

    auto vertexAttribute = std::begin(attributes);
    for (std::size_t locationIndex = 0; locationIndex < sortedElements.size(); ++locationIndex) {
        if (vertexAttribute == std::end(attributes)) {
            break;
        }

        auto& sourceElement = sortedElements[locationIndex];

        POMDOG_ASSERT(sourceElement.inputSlot >= 0);

        if (locationIndex != vertexAttribute->attributeLocation) {
            continue;
        }

        InputElementGL4 inputElement;
        inputElement.attributeLocation = vertexAttribute->attributeLocation;
        inputElement.scalarType = vertexAttribute->scalarType;
        inputElement.components = vertexAttribute->components;
        inputElement.isInteger = vertexAttribute->isInteger;
        inputElement.byteOffset = sourceElement.byteOffset;
        inputElement.instanceStepRate = sourceElement.instanceStepRate;
        inputElement.inputSlot = sourceElement.inputSlot;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
        POMDOG_ASSERT(inputElement.scalarType == ToScalarTypeGL4(sourceElement.format));
        POMDOG_ASSERT(inputElement.components == ToComponentsGL4(sourceElement.format));
        POMDOG_ASSERT(inputElement.attributeLocation < GetMaxAttributeCount());
#endif

        inputElements.push_back(std::move(inputElement));
        ++vertexAttribute;
    }

    return inputElements;
}

template <typename T>
const GLvoid* ComputeBufferOffset(T const offsetBytes)
{
    static_assert(std::is_unsigned<T>::value, "T is unsigned type.");
    return reinterpret_cast<const GLvoid*>(offsetBytes);
}

void ApplyInputElements(
    const std::vector<InputElementGL4>& inputElements,
    const std::array<VertexBufferBindingGL4, 8>& vertexBuffers)
{
    POMDOG_ASSERT(!inputElements.empty());
    POMDOG_ASSERT(!vertexBuffers.empty());

    auto inputElement = std::begin(inputElements);

    for (auto& pair : vertexBuffers) {
        if (pair.VertexBuffer == nullptr) {
            break;
        }
        if (inputElement == std::end(inputElements)) {
            break;
        }

        auto& vertexBuffer = pair.VertexBuffer;
        const auto vertexOffset = pair.VertexOffset;

        POMDOG_ASSERT(vertexBuffer);

        auto vertexBufferGL4 = dynamic_cast<VertexBufferGL4*>(vertexBuffer->getBuffer());
        POMDOG_ASSERT(vertexBufferGL4 != nullptr);

        // NOTE: The following code is the same as
        // `glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer)`.
        vertexBufferGL4->bindBuffer();

        POMDOG_ASSERT(vertexBuffer->getStrideBytes() > 0);
        POMDOG_ASSERT(vertexBuffer->getStrideBytes() <= static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()));

        const auto currentInputSlot = inputElement->inputSlot;

        for (; inputElement != std::end(inputElements); ++inputElement) {
            POMDOG_ASSERT(currentInputSlot <= inputElement->inputSlot);

            if (currentInputSlot != inputElement->inputSlot) {
                break;
            }

            if (inputElement->isInteger) {
                glVertexAttribIPointer(
                    inputElement->attributeLocation,
                    inputElement->components,
                    inputElement->scalarType.value,
                    static_cast<GLsizei>(vertexBuffer->getStrideBytes()),
                    ComputeBufferOffset(inputElement->byteOffset + vertexOffset));
                POMDOG_CHECK_ERROR_GL4("glVertexAttribIPointer");
            }
            else {
                glVertexAttribPointer(
                    inputElement->attributeLocation,
                    inputElement->components,
                    inputElement->scalarType.value,
                    GL_FALSE,
                    static_cast<GLsizei>(vertexBuffer->getStrideBytes()),
                    ComputeBufferOffset(inputElement->byteOffset + vertexOffset));
                POMDOG_CHECK_ERROR_GL4("glVertexAttribPointer");
            }

            glVertexAttribDivisor(inputElement->attributeLocation, inputElement->instanceStepRate);
            POMDOG_CHECK_ERROR_GL4("glVertexAttribDivisor");
        }
    }

    POMDOG_ASSERT(inputElement == std::end(inputElements));
}

} // namespace

template <>
struct TypesafeHelperGL4::Traits<VertexArrayGL4> {
    constexpr static GLenum BufferBinding = GL_VERTEX_ARRAY_BINDING;
};

InputLayoutGL4::InputLayoutGL4(const ShaderProgramGL4& shaderProgram)
    : InputLayoutGL4(shaderProgram, {})
{
}

InputLayoutGL4::InputLayoutGL4(
    const ShaderProgramGL4& shaderProgram,
    const InputLayoutDescriptor& descriptor)
{
    // Build vertex array object
    inputLayout = ([] {
        VertexArrayGL4 vertexArray;
        glGenVertexArrays(1, vertexArray.Data());
        return vertexArray;
    })();

    auto const oldInputLayout = TypesafeHelperGL4::Get<VertexArrayGL4>();
    ScopeGuard scope([&] { glBindVertexArray(oldInputLayout.value); });

    glBindVertexArray(inputLayout->value);
    POMDOG_CHECK_ERROR_GL4("glBindVertexArray");

    inputElements = BuildAttributes(shaderProgram);

    if (descriptor.inputElements.empty()) {
        inputElements = BuildInputElements(std::move(inputElements));
    }
    else {
        inputElements = BuildInputElements(descriptor, std::move(inputElements));
    }

    inputElements.shrink_to_fit();

    EnableAttributes(inputElements);
}

InputLayoutGL4::~InputLayoutGL4()
{
    if (inputLayout) {
        glDeleteVertexArrays(1, inputLayout->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteVertexArrays");
    }
}

void InputLayoutGL4::Apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers)
{
    POMDOG_ASSERT(inputLayout);
    glBindVertexArray(inputLayout->value);
    POMDOG_CHECK_ERROR_GL4("glBindVertexArray");

    ApplyInputElements(inputElements, vertexBuffers);
}

} // namespace pomdog::gpu::detail::gl4
