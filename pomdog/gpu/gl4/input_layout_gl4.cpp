// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/input_layout_desc.h"
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

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] ScalarTypeGL4
toScalarType(GLenum attributeClass)
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

[[nodiscard]] bool
isIntegerType(const ScalarTypeGL4& scalarType)
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
    u8 semanticIndex;

    ///@note
    /// float: 1
    /// Vector2: 2
    /// Vector3: 3
    /// Vector4: 4
    /// Matrix2x2: 2
    /// Matrix3x3: 3
    /// Matrix4x4: 4
    i8 componentCount;
};

[[nodiscard]] InputElementSize
toInputElementSize(GLenum attributeClass)
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

[[nodiscard]] u8
toByteWithFromScalarTypeGL4(ScalarTypeGL4 scalarType)
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

[[nodiscard]] std::vector<InputElementGL4>
buildAttributes(const ShaderProgramGL4& shaderProgram)
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
        auto const attributeScalarType = toScalarType(attributeClass);
        auto const attributeSize = toInputElementSize(attributeClass);

        POMDOG_ASSERT(attributeSize.componentCount >= 1 && attributeSize.componentCount <= 4);
        POMDOG_ASSERT(attributeSize.semanticIndex >= 1 && attributeSize.semanticIndex <= 4);

        for (u16 row = 0; row < attributeSize.semanticIndex * attributeVariableArraySize; ++row) {
            InputElementGL4 attribute;
            attribute.attributeLocation = attributeLocation;
            attribute.scalarType = attributeScalarType;
            attribute.components = attributeSize.componentCount;
            attribute.isInteger = isIntegerType(attributeScalarType);
            attribute.byteOffset = 0; // NOTE: See calculateByteOffset function.
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

void enableAttributes(const std::vector<InputElementGL4>& inputElements)
{
    for (auto& inputElement : inputElements) {
        glEnableVertexAttribArray(inputElement.attributeLocation);
        POMDOG_CHECK_ERROR_GL4("glEnableVertexAttribArray");
    }
}

[[nodiscard]] u32
calculateByteOffset(const ScalarTypeGL4& scalarType, i8 components)
{
    POMDOG_ASSERT(components >= 1);
    POMDOG_ASSERT(components <= 4);

    return toByteWithFromScalarTypeGL4(scalarType) * static_cast<u32>(components);
}

[[nodiscard]] std::vector<InputElementGL4>
buildInputElements(std::vector<InputElementGL4>&& inputElements)
{
    POMDOG_ASSERT(!inputElements.empty());

    u32 offsetBytes = 0;
    for (auto& inputElement : inputElements) {
        inputElement.byteOffset = offsetBytes;
        inputElement.instanceStepRate = 0;
        inputElement.inputSlot = 0;
        offsetBytes += calculateByteOffset(inputElement.scalarType, inputElement.components);
        POMDOG_ASSERT(offsetBytes > 0);
    }

    return std::move(inputElements);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
[[nodiscard]] ScalarTypeGL4
toScalarTypeGL4(InputElementFormat format)
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

[[nodiscard]] i8
toComponentsGL4(InputElementFormat format)
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

[[nodiscard]] GLuint
getMaxAttributeCount()
{
    static const GLuint maxAttributeCount = ([] {
        GLint maxCount = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxCount);
        return maxCount;
    })();
    return maxAttributeCount;
}
#endif

[[nodiscard]] std::vector<InputElementGL4>
buildInputElements(
    const InputLayoutDesc& descriptor,
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
        POMDOG_ASSERT(inputElement.scalarType == toScalarTypeGL4(sourceElement.format));
        POMDOG_ASSERT(inputElement.components == toComponentsGL4(sourceElement.format));
        POMDOG_ASSERT(inputElement.attributeLocation < getMaxAttributeCount());
#endif

        inputElements.push_back(std::move(inputElement));
        ++vertexAttribute;
    }

    return inputElements;
}

[[nodiscard]] const GLvoid*
computeBufferOffset(std::size_t offsetBytes)
{
    return reinterpret_cast<const GLvoid*>(offsetBytes);
}

void applyInputElements(
    const std::vector<InputElementGL4>& inputElements,
    const std::array<VertexBufferBindingGL4, 8>& vertexBuffers)
{
    POMDOG_ASSERT(!inputElements.empty());
    POMDOG_ASSERT(!vertexBuffers.empty());

    auto inputElement = std::begin(inputElements);

    for (auto& pair : vertexBuffers) {
        if (pair.vertexBuffer == nullptr) {
            break;
        }
        if (inputElement == std::end(inputElements)) {
            break;
        }

        auto& vertexBuffer = pair.vertexBuffer;
        const auto vertexOffset = pair.vertexOffset;

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
                    computeBufferOffset(inputElement->byteOffset + vertexOffset));
                POMDOG_CHECK_ERROR_GL4("glVertexAttribIPointer");
            }
            else {
                glVertexAttribPointer(
                    inputElement->attributeLocation,
                    inputElement->components,
                    inputElement->scalarType.value,
                    GL_FALSE,
                    static_cast<GLsizei>(vertexBuffer->getStrideBytes()),
                    computeBufferOffset(inputElement->byteOffset + vertexOffset));
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
struct TypesafeHelperGL4::Traits<VertexArrayGL4> final {
    constexpr static GLenum BufferBinding = GL_VERTEX_ARRAY_BINDING;
};

InputLayoutGL4::InputLayoutGL4(const ShaderProgramGL4& shaderProgram)
    : InputLayoutGL4(shaderProgram, {})
{
}

InputLayoutGL4::InputLayoutGL4(
    const ShaderProgramGL4& shaderProgram,
    const InputLayoutDesc& descriptor)
{
    // Build vertex array object
    inputLayout_ = ([] {
        VertexArrayGL4 vertexArray;
        glGenVertexArrays(1, vertexArray.Data());
        return vertexArray;
    })();

    auto const oldInputLayout = TypesafeHelperGL4::Get<VertexArrayGL4>();
    ScopeGuard scope([&] { glBindVertexArray(oldInputLayout.value); });

    glBindVertexArray(inputLayout_->value);
    POMDOG_CHECK_ERROR_GL4("glBindVertexArray");

    inputElements_ = buildAttributes(shaderProgram);

    if (descriptor.inputElements.empty()) {
        inputElements_ = buildInputElements(std::move(inputElements_));
    }
    else {
        inputElements_ = buildInputElements(descriptor, std::move(inputElements_));
    }

    inputElements_.shrink_to_fit();

    enableAttributes(inputElements_);
}

InputLayoutGL4::~InputLayoutGL4()
{
    if (inputLayout_) {
        glDeleteVertexArrays(1, inputLayout_->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteVertexArrays");
    }
}

void InputLayoutGL4::apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers)
{
    POMDOG_ASSERT(inputLayout_);
    glBindVertexArray(inputLayout_->value);
    POMDOG_CHECK_ERROR_GL4("glBindVertexArray");

    applyInputElements(inputElements_, vertexBuffers);
}

} // namespace pomdog::gpu::detail::gl4
