// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/effect_reflection_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <tuple>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
toPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleList:
        return GL_TRIANGLES;
    case PrimitiveTopology::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case PrimitiveTopology::LineList:
        return GL_LINES;
    case PrimitiveTopology::LineStrip:
        return GL_LINE_STRIP;
    }
    POMDOG_UNREACHABLE("Unsupported primitive topology");
}

[[nodiscard]] std::tuple<std::optional<ShaderProgramGL4>, std::unique_ptr<Error>>
linkShaders(const VertexShaderGL4& vertexShader, const PixelShaderGL4& pixelShader) noexcept
{
    ShaderProgramGL4 const program{glCreateProgram()};

    glAttachShader(program.value, vertexShader.getShader());
    glAttachShader(program.value, pixelShader.getShader());

    glLinkProgram(program.value);
    POMDOG_CHECK_ERROR_GL4("glLinkProgram");

    GLint linkSuccess = 0;
    glGetProgramiv(program.value, GL_LINK_STATUS, &linkSuccess);

    if (linkSuccess == GL_FALSE) {
        std::array<GLchar, 256> messageBuffer;

        glGetProgramInfoLog(program.value, static_cast<GLsizei>(messageBuffer.size()), nullptr, messageBuffer.data());
        messageBuffer.back() = '\0';
        const std::string message = messageBuffer.data();

        glDeleteProgram(program.value);
        return std::make_tuple(std::nullopt, errors::make("failed to link shaders: " + message));
    }

    return std::make_tuple(std::move(program), nullptr);
}

[[nodiscard]] std::unique_ptr<Error>
prepareUniformBlocksWithReflection(
    const VertexShaderGL4& vertexShader,
    const PixelShaderGL4& pixelShader,
    ShaderProgramGL4 shaderProgram) noexcept
{
    auto uniformBlocks = enumerateUniformBlocks(shaderProgram);

    for (auto& uniformBlock : uniformBlocks) {
        std::optional<u8> slotIndex;

        auto vs = vertexShader.findConstantBufferSlotIndex(uniformBlock.name);
        if (vs != std::nullopt) {
            slotIndex = vs;
        }

        auto ps = pixelShader.findConstantBufferSlotIndex(uniformBlock.name);
        if (ps != std::nullopt) {
            if (slotIndex != std::nullopt && *slotIndex != *ps) {
                return errors::make("slot index mismatch between vertex and pixel shader for uniform block: " + uniformBlock.name);
            }
            slotIndex = ps;
        }

        if (slotIndex == std::nullopt) {
            return errors::make("slot index not found for uniform block: " + uniformBlock.name);
        }

        glUniformBlockBinding(shaderProgram.value, uniformBlock.blockIndex, static_cast<GLuint>(*slotIndex));
        POMDOG_CHECK_ERROR_GL4("glUniformBlockBinding");
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
prepareUniformsWithReflection(
    const VertexShaderGL4& vertexShader,
    const PixelShaderGL4& pixelShader,
    ShaderProgramGL4 shaderProgram,
    std::vector<TextureBindingGL4>& textureBindings) noexcept
{
    auto uniforms = enumerateUniforms(shaderProgram);

    for (auto& uniform : uniforms) {
        switch (uniform.type) {
        case GL_SAMPLER_1D:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_1D_SHADOW:
        case GL_SAMPLER_2D_SHADOW:
        case GL_SAMPLER_1D_ARRAY:
        case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_1D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_SAMPLER_CUBE_SHADOW:
        case GL_SAMPLER_BUFFER:
        case GL_SAMPLER_2D_RECT:
        case GL_SAMPLER_2D_RECT_SHADOW:
            break;
        default:
            continue;
        }

        std::optional<u8> slotIndex;

        auto vs = vertexShader.findSamplerSlotIndex(uniform.name);
        if (vs != std::nullopt) {
            slotIndex = vs;
        }

        auto ps = pixelShader.findSamplerSlotIndex(uniform.name);
        if (ps != std::nullopt) {
            if (slotIndex != std::nullopt && *slotIndex != *ps) {
                return errors::make("slot index mismatch between vertex and pixel shader for sampler: " + uniform.name);
            }
            slotIndex = ps;
        }

        if (slotIndex == std::nullopt) {
            return errors::make("slot index not found for sampler: " + uniform.name);
        }

        TextureBindingGL4 binding;
        binding.uniformLocation = uniform.location;
        binding.slotIndex = *slotIndex;
        textureBindings.push_back(binding);
    }

    return nullptr;
}

} // namespace

PipelineStateGL4::PipelineStateGL4() = default;

std::unique_ptr<Error>
PipelineStateGL4::initialize(const PipelineDesc& descriptor) noexcept
{
    if (auto err = blendState_.initialize(descriptor.blendState); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize blendState");
    }
    if (auto err = rasterizerState_.initialize(descriptor.rasterizerState); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize rasterizerState");
    }
    if (auto err = depthStencilState_.initialize(descriptor.depthStencilState); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize depthStencilState");
    }

    primitiveTopology_ = toPrimitiveTopology(descriptor.primitiveTopology);

    auto vertexShader = std::dynamic_pointer_cast<VertexShaderGL4>(descriptor.vertexShader);
    if (vertexShader == nullptr) {
        return errors::make("invalid vertex shader");
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderGL4>(descriptor.pixelShader);
    if (pixelShader == nullptr) {
        return errors::make("invalid pixel shader");
    }

    POMDOG_ASSERT(vertexShader);
    POMDOG_ASSERT(pixelShader);

    auto [linkResult, linkErr] = linkShaders(*vertexShader, *pixelShader);
    if (linkErr != nullptr) {
        return errors::wrap(std::move(linkErr), "failed to link shader program");
    }
    shaderProgram_ = std::move(linkResult);
    POMDOG_ASSERT(shaderProgram_ != std::nullopt);

    inputLayout_ = std::make_unique<InputLayoutGL4>();
    if (auto err = inputLayout_->initialize(*shaderProgram_, descriptor.inputLayout); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize input layout");
    }

    if (auto err = prepareUniformBlocksWithReflection(*vertexShader, *pixelShader, *shaderProgram_); err != nullptr) {
        return errors::wrap(std::move(err), "prepareUniformBlocksWithReflection() failed");
    }
    if (auto err = prepareUniformsWithReflection(*vertexShader, *pixelShader, *shaderProgram_, textureBindings_); err != nullptr) {
        return errors::wrap(std::move(err), "prepareUniformsWithReflection() failed");
    }

    return nullptr;
}

PipelineStateGL4::~PipelineStateGL4()
{
    if (shaderProgram_) {
        glDeleteProgram(shaderProgram_->value);
        POMDOG_CHECK_ERROR_GL4("glDeleteProgram");
    }
}

void PipelineStateGL4::applyShaders()
{
    blendState_.apply();
    rasterizerState_.apply();
    depthStencilState_.apply();

    POMDOG_ASSERT(shaderProgram_);
    glUseProgram(shaderProgram_->value);
    POMDOG_CHECK_ERROR_GL4("glUseProgram");

    for (auto& binding : textureBindings_) {
        glUniform1i(binding.uniformLocation, binding.slotIndex);
        POMDOG_CHECK_ERROR_GL4("glUniform1i");
    }
}

ShaderProgramGL4
PipelineStateGL4::getShaderProgram() const noexcept
{
    POMDOG_ASSERT(shaderProgram_);
    return *shaderProgram_;
}

unsafe_ptr<InputLayoutGL4>
PipelineStateGL4::getInputLayout() const noexcept
{
    POMDOG_ASSERT(inputLayout_);
    return inputLayout_.get();
}

PrimitiveTopologyGL4
PipelineStateGL4::getPrimitiveTopology() const noexcept
{
    return primitiveTopology_;
}

} // namespace pomdog::gpu::detail::gl4
