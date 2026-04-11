// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] std::tuple<ScalarTypeGL4, std::unique_ptr<Error>>
toScalarType(GLenum attributeClass) noexcept
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
        return std::make_tuple(ScalarTypeGL4(GL_FLOAT), nullptr);

    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
        return std::make_tuple(ScalarTypeGL4(GL_INT), nullptr);

    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
        return std::make_tuple(ScalarTypeGL4(GL_UNSIGNED_INT), nullptr);

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
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
        return std::make_tuple(ScalarTypeGL4(GL_DOUBLE), nullptr);
#endif

    case GL_BYTE:
        return std::make_tuple(ScalarTypeGL4(GL_BYTE), nullptr);

    case GL_UNSIGNED_BYTE:
        return std::make_tuple(ScalarTypeGL4(GL_UNSIGNED_BYTE), nullptr);

    case GL_SHORT:
        return std::make_tuple(ScalarTypeGL4(GL_SHORT), nullptr);

    case GL_UNSIGNED_SHORT:
        return std::make_tuple(ScalarTypeGL4(GL_UNSIGNED_SHORT), nullptr);
    }

    return std::make_tuple(ScalarTypeGL4(GL_FLOAT), errors::make("attribute class '" + std::to_string(attributeClass) + "' is not supported"));
}

struct ScalarTypeInfo final {
    ScalarTypeGL4 scalarType;
    bool isInteger;
    bool normalized;
};

[[nodiscard]] ScalarTypeInfo
toScalarTypeInfo(InputElementFormat format)
{
    switch (format) {
    case InputElementFormat::Float32x1:
    case InputElementFormat::Float32x2:
    case InputElementFormat::Float32x3:
    case InputElementFormat::Float32x4:
        return {ScalarTypeGL4(GL_FLOAT), false, false};
    case InputElementFormat::Uint8x1:
    case InputElementFormat::Uint8x2:
    case InputElementFormat::Uint8x4:
        return {ScalarTypeGL4(GL_UNSIGNED_BYTE), true, false};
    case InputElementFormat::Unorm8x1:
    case InputElementFormat::Unorm8x2:
    case InputElementFormat::Unorm8x4:
        return {ScalarTypeGL4(GL_UNSIGNED_BYTE), false, true};
    case InputElementFormat::Int32x1:
    case InputElementFormat::Int32x2:
    case InputElementFormat::Int32x3:
    case InputElementFormat::Int32x4:
        return {ScalarTypeGL4(GL_INT), true, false};
    case InputElementFormat::Float16x2:
    case InputElementFormat::Float16x4:
        return {ScalarTypeGL4(GL_HALF_FLOAT), false, false};
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

[[nodiscard]] bool
isIntegerType(const ScalarTypeGL4& scalarType) noexcept
{
    switch (scalarType.value) {
    case GL_FLOAT:
    case GL_HALF_FLOAT:
        return false;
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    case GL_DOUBLE:
        return false;
#endif
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return true;
    }
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
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return {1, 1};

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
        return {1, 2};

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
        return {1, 3};

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
        return {1, 4};

    case GL_FLOAT_MAT2:
        return {2, 2};

    case GL_FLOAT_MAT3:
        return {3, 3};

    case GL_FLOAT_MAT4:
        return {4, 4};

    case GL_FLOAT_MAT2x3:
        return {2, 3};

    case GL_FLOAT_MAT3x2:
        return {3, 2};

    case GL_FLOAT_MAT2x4:
        return {2, 4};

    case GL_FLOAT_MAT4x2:
        return {4, 2};

    case GL_FLOAT_MAT3x4:
        return {3, 4};

    case GL_FLOAT_MAT4x3:
        return {4, 3};

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    case GL_DOUBLE:
        return {1, 1};

    case GL_DOUBLE_VEC2:
        return {1, 2};

    case GL_DOUBLE_VEC3:
        return {1, 3};

    case GL_DOUBLE_VEC4:
        return {1, 4};

    case GL_DOUBLE_MAT2:
        return {2, 2};

    case GL_DOUBLE_MAT3:
        return {3, 3};

    case GL_DOUBLE_MAT4:
        return {4, 4};

    case GL_DOUBLE_MAT2x3:
        return {2, 3};

    case GL_DOUBLE_MAT3x2:
        return {3, 2};

    case GL_DOUBLE_MAT2x4:
        return {2, 4};

    case GL_DOUBLE_MAT4x2:
        return {4, 2};

    case GL_DOUBLE_MAT3x4:
        return {3, 4};

    case GL_DOUBLE_MAT4x3:
        return {4, 3};
#endif
    }

    return {1, 1};
}

[[nodiscard]] InputElementSize
toInputElementSize(InputElementFormat format)
{
    switch (format) {
    case InputElementFormat::Float32x1:
    case InputElementFormat::Uint8x1:
    case InputElementFormat::Unorm8x1:
    case InputElementFormat::Int32x1:
        return {1, 1};
    case InputElementFormat::Float32x2:
    case InputElementFormat::Uint8x2:
    case InputElementFormat::Unorm8x2:
    case InputElementFormat::Int32x2:
    case InputElementFormat::Float16x2:
        return {1, 2};
    case InputElementFormat::Float32x3:
    case InputElementFormat::Int32x3:
        return {1, 3};
    case InputElementFormat::Float32x4:
    case InputElementFormat::Uint8x4:
    case InputElementFormat::Unorm8x4:
    case InputElementFormat::Int32x4:
    case InputElementFormat::Float16x4:
        return {1, 4};
    }
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
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    case GL_DOUBLE:
        return sizeof(GLdouble);
#endif
    case GL_BYTE:
        return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    case GL_SHORT:
        return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
    }
    return sizeof(float);
}

struct ShaderAttribute final {
    GLuint attributeLocation;
    ScalarTypeGL4 scalarType;
    i8 componentCount;
    bool isInteger;
};

[[nodiscard]] std::tuple<std::vector<ShaderAttribute>, std::unique_ptr<Error>>
buildAttributes(const ShaderProgramGL4& shaderProgram) noexcept
{
    std::vector<ShaderAttribute> attributes;

    GLint attributeCount = 0;
    GLint maxAttributeLength = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTES, &attributeCount);
    if (auto err = getLastError(); err != nullptr) {
        return std::make_tuple(std::move(attributes), errors::wrap(std::move(err), "glGetProgramiv(GL_ACTIVE_ATTRIBUTES) failed"));
    }
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength);
    if (auto err = getLastError(); err != nullptr) {
        return std::make_tuple(std::move(attributes), errors::wrap(std::move(err), "glGetProgramiv(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH) failed"));
    }

    if ((attributeCount < 0) || (maxAttributeLength <= 0)) {
        return std::make_tuple(std::move(attributes), errors::make("invalid vertex attributes"));
    }

    for (GLint i = 0; i < attributeCount; ++i) {
        GLenum attributeClass = 0;
        GLint attributeVariableArraySize = 0;
        std::vector<GLchar> name(static_cast<std::size_t>(maxAttributeLength) + 1, '\0');

        // Get attribute variable
        glGetActiveAttrib(shaderProgram.value, i, maxAttributeLength, nullptr,
            &attributeVariableArraySize, &attributeClass, name.data());

        // Get attribute location
        auto attributeLocation = glGetAttribLocation(shaderProgram.value, name.data());

        // Get scalar type
        auto [attributeScalarType, scalarTypeErr] = toScalarType(attributeClass);
        if (scalarTypeErr != nullptr) {
            return std::make_tuple(std::move(attributes), errors::wrap(std::move(scalarTypeErr), "toScalarType() failed"));
        }
        auto const attributeSize = toInputElementSize(attributeClass);

        POMDOG_ASSERT(attributeSize.componentCount >= 1 && attributeSize.componentCount <= 4);
        POMDOG_ASSERT(attributeSize.semanticIndex >= 1 && attributeSize.semanticIndex <= 4);

        for (u16 row = 0; row < attributeSize.semanticIndex * attributeVariableArraySize; ++row) {
            ShaderAttribute attribute;
            attribute.attributeLocation = attributeLocation;
            attribute.scalarType = attributeScalarType;
            attribute.componentCount = attributeSize.componentCount;
            attribute.isInteger = isIntegerType(attributeScalarType);

            POMDOG_ASSERT(attribute.componentCount >= 1 && attribute.componentCount <= 4);
            attributes.push_back(attribute);

            // FIXME: badcode
            // For matrix class in GLSL:
            attributeLocation += 1;
        }
    }

    std::stable_sort(std::begin(attributes), std::end(attributes), [](const ShaderAttribute& a, const ShaderAttribute& b) {
        return a.attributeLocation < b.attributeLocation;
    });

    return std::make_tuple(std::move(attributes), nullptr);
}

[[nodiscard]] u32
calculateByteOffset(const ScalarTypeGL4& scalarType, i8 components)
{
    POMDOG_ASSERT(components >= 1);
    POMDOG_ASSERT(components <= 4);

    return toByteWithFromScalarTypeGL4(scalarType) * static_cast<u32>(components);
}

[[nodiscard]] std::vector<VertexBufferLayoutGL4>
buildVertexBuffersFromReflection(std::vector<ShaderAttribute>&& attributes)
{
    POMDOG_ASSERT(!attributes.empty());

    VertexBufferLayoutGL4 bufferLayout = {};
    bufferLayout.inputSlot = 0;
    bufferLayout.instanceStepRate = 0;
    bufferLayout.strideBytes = 0;

    u32 offsetBytes = 0;
    for (auto& attr : attributes) {
        InputElementGL4 element = {};
        element.attributeLocation = attr.attributeLocation;
        element.scalarType = attr.scalarType;
        element.components = attr.componentCount;
        element.isInteger = attr.isInteger;
        element.normalized = false;
        element.byteOffset = offsetBytes;
        offsetBytes += calculateByteOffset(attr.scalarType, attr.componentCount);
        POMDOG_ASSERT(offsetBytes > 0);

        bufferLayout.elements.push_back(std::move(element));
    }
    bufferLayout.strideBytes = offsetBytes;

    std::vector<VertexBufferLayoutGL4> result = {};
    result.push_back(std::move(bufferLayout));
    return result;
}

[[nodiscard]] std::vector<VertexBufferLayoutGL4>
buildVertexBuffersFromDescriptor(
    const InputLayoutDesc& descriptor,
    std::vector<ShaderAttribute>&& attributes)
{
    POMDOG_ASSERT(!descriptor.vertexBuffers.empty());
    POMDOG_ASSERT(!attributes.empty());

    std::vector<VertexBufferLayoutGL4> result = {};
    result.reserve(descriptor.vertexBuffers.size());

    auto vertexAttribute = std::begin(attributes);
    u32 semanticIndex = 0;

    for (auto& bufferDesc : descriptor.vertexBuffers) {
        VertexBufferLayoutGL4 nativeBufferLayout;
        nativeBufferLayout.inputSlot = bufferDesc.inputSlot;
        nativeBufferLayout.strideBytes = bufferDesc.strideBytes;
        nativeBufferLayout.instanceStepRate = bufferDesc.instanceStepRate;

        nativeBufferLayout.elements.reserve(bufferDesc.elements.size());
        for (auto& element : bufferDesc.elements) {
            ScopeGuard incrementDefer([&] { semanticIndex += 1; });

            if (vertexAttribute == std::end(attributes)) {
                break;
            }

            if (semanticIndex != vertexAttribute->attributeLocation) {
                continue;
            }

            auto typeInfo = toScalarTypeInfo(element.format);
            auto dimension = toInputElementSize(element.format);

            InputElementGL4 nativeElement;
            nativeElement.attributeLocation = vertexAttribute->attributeLocation;
            nativeElement.byteOffset = element.byteOffset;
            nativeElement.scalarType = typeInfo.scalarType;
            nativeElement.components = dimension.componentCount;
            nativeElement.isInteger = typeInfo.isInteger;
            nativeElement.normalized = typeInfo.normalized;
            nativeBufferLayout.elements.push_back(std::move(nativeElement));

            ++vertexAttribute;
        }
        nativeBufferLayout.elements.shrink_to_fit();
        result.push_back(std::move(nativeBufferLayout));
    }

    return result;
}

[[nodiscard]] const GLvoid*
computeBufferOffset(std::size_t offsetBytes)
{
    return reinterpret_cast<const GLvoid*>(offsetBytes);
}

void applyVertexBuffers(
    std::span<const VertexBufferLayoutGL4> vertexBufferLayouts,
    const std::array<VertexBufferBindingGL4, 8>& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBufferLayouts.empty());
    POMDOG_ASSERT(!vertexBuffers.empty());

    for (auto& bufferLayout : vertexBufferLayouts) {
        POMDOG_ASSERT(bufferLayout.inputSlot < vertexBuffers.size());
        auto& pair = vertexBuffers[bufferLayout.inputSlot];

        if (pair.vertexBuffer == nullptr) {
            continue;
        }

        auto& vertexBuffer = pair.vertexBuffer;
        const auto vertexOffset = pair.vertexOffset;

        POMDOG_ASSERT(vertexBuffer);

        auto vertexBufferGL4 = dynamic_cast<BufferGL4*>(vertexBuffer->getBuffer());
        POMDOG_ASSERT(vertexBufferGL4 != nullptr);

        // NOTE: The following code is the same as
        // `glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer)`.
        vertexBufferGL4->bindBuffer();

        POMDOG_ASSERT(bufferLayout.strideBytes > 0);

        for (auto& element : bufferLayout.elements) {
            if (element.isInteger) {
                glVertexAttribIPointer(
                    element.attributeLocation,
                    element.components,
                    element.scalarType.value,
                    static_cast<GLsizei>(bufferLayout.strideBytes),
                    computeBufferOffset(element.byteOffset + vertexOffset));
                POMDOG_CHECK_ERROR_GL4("glVertexAttribIPointer");
            }
            else {
                glVertexAttribPointer(
                    element.attributeLocation,
                    element.components,
                    element.scalarType.value,
                    element.normalized ? GL_TRUE : GL_FALSE,
                    static_cast<GLsizei>(bufferLayout.strideBytes),
                    computeBufferOffset(element.byteOffset + vertexOffset));
                POMDOG_CHECK_ERROR_GL4("glVertexAttribPointer");
            }

            glVertexAttribDivisor(element.attributeLocation, bufferLayout.instanceStepRate);
            POMDOG_CHECK_ERROR_GL4("glVertexAttribDivisor");
        }
    }
}

} // namespace

template <>
struct TypesafeHelperGL4::Traits<VertexArrayGL4> final {
    constexpr static GLenum BufferBinding = GL_VERTEX_ARRAY_BINDING;
};

InputLayoutGL4::InputLayoutGL4() = default;

std::unique_ptr<Error>
InputLayoutGL4::initialize(
    const ShaderProgramGL4& shaderProgram,
    const InputLayoutDesc& descriptor) noexcept
{
    // Build vertex array object
    inputLayout_ = ([] {
        VertexArrayGL4 vertexArray;
        glGenVertexArrays(1, vertexArray.Data());
        return vertexArray;
    })();
    if (auto err = getLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create vertex array object");
    }

    auto const oldInputLayout = TypesafeHelperGL4::Get<VertexArrayGL4>();
    ScopeGuard scope([&] { glBindVertexArray(oldInputLayout.value); });

    glBindVertexArray(inputLayout_->value);
    if (auto err = getLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to bind vertex array object");
    }

    auto [attributes, attrErr] = buildAttributes(shaderProgram);
    if (attrErr != nullptr) {
        return errors::wrap(std::move(attrErr), "failed to build attributes from shader program");
    }

    if (descriptor.vertexBuffers.empty()) {
        vertexBuffers_ = buildVertexBuffersFromReflection(std::move(attributes));
    }
    else {
        vertexBuffers_ = buildVertexBuffersFromDescriptor(descriptor, std::move(attributes));
    }

    for (auto& bufferLayout : vertexBuffers_) {
        for (auto& element : bufferLayout.elements) {
            glEnableVertexAttribArray(element.attributeLocation);
            if (auto err = getLastError(); err != nullptr) {
                return errors::wrap(std::move(err), "glEnableVertexAttribArray() failed");
            }
        }
    }

    return nullptr;
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

    applyVertexBuffers(vertexBuffers_, vertexBuffers);
}

} // namespace pomdog::gpu::detail::gl4
