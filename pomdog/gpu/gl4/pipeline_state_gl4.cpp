// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/effect_reflection_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <tuple>
#include <type_traits>
#include <unordered_set>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

// NOTE: Please refer to D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT.
constexpr std::size_t ConstantBufferSlotCount = 14;

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

} // namespace

PipelineStateGL4::PipelineStateGL4() = default;

std::unique_ptr<Error>
PipelineStateGL4::initialize(const PipelineDescriptor& descriptor) noexcept
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

    inputLayout_ = std::make_unique<InputLayoutGL4>(*shaderProgram_, descriptor.inputLayout);

    EffectReflectionGL4 shaderReflection;
    if (auto err = shaderReflection.initialize(*shaderProgram_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize EffectReflectionGL4");
    }

    {
        auto uniformBlocks = shaderReflection.getNativeUniformBlocks();

        std::unordered_set<int> reservedSlots;
        std::unordered_set<int> reservedBlocks;

        auto& bindSlots = descriptor.constantBufferBindHints;

        for (auto& uniformBlock : uniformBlocks) {
            auto binding = bindSlots.find(uniformBlock.name);
            if (binding != std::end(bindSlots)) {
                auto slotIndex = binding->second;
                glUniformBlockBinding(shaderProgram_->value, uniformBlock.blockIndex, slotIndex);
                reservedSlots.insert(slotIndex);
                reservedBlocks.insert(uniformBlock.blockIndex);
            }
        }

        GLuint slotIndex = 0;
        for (auto& uniformBlock : uniformBlocks) {
            if (reservedBlocks.find(uniformBlock.blockIndex) != std::end(reservedBlocks)) {
                continue;
            }
            while (reservedSlots.find(slotIndex) != std::end(reservedSlots)) {
                if (slotIndex >= ConstantBufferSlotCount) {
                    break;
                }
                ++slotIndex;
            }
            glUniformBlockBinding(shaderProgram_->value, uniformBlock.blockIndex, slotIndex);
            ++slotIndex;
        }
    }
    {
        auto uniforms = shaderReflection.getNativeUniforms();

        auto& hints = descriptor.samplerBindHints;

        u16 slotIndex = 0;
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

            if (auto hint = hints.find(uniform.name); hint != std::end(hints)) {
                TextureBindingGL4 binding;
                binding.uniformLocation = uniform.location;
                binding.slotIndex = static_cast<u16>(hint->second);
                static_assert(std::is_same_v<decltype(binding.slotIndex), u16>);

                auto iter = std::find_if(
                    std::begin(textureBindings_),
                    std::end(textureBindings_),
                    [&](auto& t) { return t.slotIndex == binding.slotIndex; });

                if (iter != std::end(textureBindings_)) {
                    iter->slotIndex = slotIndex;
                }

                textureBindings_.push_back(binding);
            }
            else {
                TextureBindingGL4 binding;
                binding.uniformLocation = uniform.location;
                binding.slotIndex = slotIndex;
                textureBindings_.push_back(binding);
            }

            ++slotIndex;
        }
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
