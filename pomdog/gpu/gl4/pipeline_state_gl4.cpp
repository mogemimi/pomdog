// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
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
#include <array>
#include <tuple>
#include <type_traits>
#include <unordered_set>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gl4 {
namespace {

// NOTE: Please refer to D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT.
constexpr std::size_t ConstantBufferSlotCount = 14;

GLenum ToPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
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
LinkShaders(const VertexShaderGL4& vertexShader, const PixelShaderGL4& pixelShader) noexcept
{
    ShaderProgramGL4 const program{glCreateProgram()};

    glAttachShader(program.value, vertexShader.GetShader());
    glAttachShader(program.value, pixelShader.GetShader());

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
        return std::make_tuple(std::nullopt, errors::New("failed to link shaders: " + message));
    }

    return std::make_tuple(std::move(program), nullptr);
}

} // namespace

PipelineStateGL4::PipelineStateGL4() = default;

std::unique_ptr<Error>
PipelineStateGL4::Initialize(const PipelineStateDescriptor& descriptor) noexcept
{
    if (auto err = blendState.Initialize(descriptor.BlendState); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize blendState");
    }
    if (auto err = rasterizerState.Initialize(descriptor.RasterizerState); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize rasterizerState");
    }
    if (auto err = depthStencilState.Initialize(descriptor.DepthStencilState); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize depthStencilState");
    }

    primitiveTopology = ToPrimitiveTopology(descriptor.PrimitiveTopology);

    auto vertexShader = std::dynamic_pointer_cast<VertexShaderGL4>(descriptor.VertexShader);
    if (vertexShader == nullptr) {
        return errors::New("invalid vertex shader");
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderGL4>(descriptor.PixelShader);
    if (pixelShader == nullptr) {
        return errors::New("invalid pixel shader");
    }

    POMDOG_ASSERT(vertexShader);
    POMDOG_ASSERT(pixelShader);

    auto [linkResult, linkErr] = LinkShaders(*vertexShader, *pixelShader);
    if (linkErr != nullptr) {
        return errors::Wrap(std::move(linkErr), "failed to link shader program");
    }
    shaderProgram = std::move(linkResult);
    POMDOG_ASSERT(shaderProgram != std::nullopt);

    inputLayout = std::make_unique<InputLayoutGL4>(*shaderProgram, descriptor.InputLayout);

    EffectReflectionGL4 shaderReflection;
    if (auto err = shaderReflection.Initialize(*shaderProgram); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize EffectReflectionGL4");
    }

    {
        auto uniformBlocks = shaderReflection.GetNativeUniformBlocks();

        std::unordered_set<int> reservedSlots;
        std::unordered_set<int> reservedBlocks;

        auto& bindSlots = descriptor.ConstantBufferBindHints;

        for (auto& uniformBlock : uniformBlocks) {
            auto binding = bindSlots.find(uniformBlock.Name);
            if (binding != std::end(bindSlots)) {
                auto slotIndex = binding->second;
                glUniformBlockBinding(shaderProgram->value, uniformBlock.BlockIndex, slotIndex);
                reservedSlots.insert(slotIndex);
                reservedBlocks.insert(uniformBlock.BlockIndex);
            }
        }

        GLuint slotIndex = 0;
        for (auto& uniformBlock : uniformBlocks) {
            if (reservedBlocks.find(uniformBlock.BlockIndex) != std::end(reservedBlocks)) {
                continue;
            }
            while (reservedSlots.find(slotIndex) != std::end(reservedSlots)) {
                if (slotIndex >= ConstantBufferSlotCount) {
                    break;
                }
                ++slotIndex;
            }
            glUniformBlockBinding(shaderProgram->value, uniformBlock.BlockIndex, slotIndex);
            ++slotIndex;
        }
    }
    {
        auto uniforms = shaderReflection.GetNativeUniforms();

        auto& hints = descriptor.SamplerBindHints;

        std::uint16_t slotIndex = 0;
        for (auto& uniform : uniforms) {
            switch (uniform.Type) {
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

            if (auto hint = hints.find(uniform.Name); hint != std::end(hints)) {
                TextureBindingGL4 binding;
                binding.UniformLocation = uniform.Location;
                binding.SlotIndex = static_cast<std::uint16_t>(hint->second);
                static_assert(std::is_same_v<decltype(binding.SlotIndex), std::uint16_t>);

                auto iter = std::find_if(
                    std::begin(textureBindings),
                    std::end(textureBindings),
                    [&](auto& t) { return t.SlotIndex == binding.SlotIndex; });

                if (iter != std::end(textureBindings)) {
                    iter->SlotIndex = slotIndex;
                }

                textureBindings.push_back(binding);
            }
            else {
                TextureBindingGL4 binding;
                binding.UniformLocation = uniform.Location;
                binding.SlotIndex = slotIndex;
                textureBindings.push_back(binding);
            }

            ++slotIndex;
        }
    }
    return nullptr;
}

PipelineStateGL4::~PipelineStateGL4()
{
    if (shaderProgram) {
        glDeleteProgram(shaderProgram->value);
        POMDOG_CHECK_ERROR_GL4("glDeleteProgram");
    }
}

void PipelineStateGL4::ApplyShaders()
{
    blendState.Apply();
    rasterizerState.Apply();
    depthStencilState.Apply();

    POMDOG_ASSERT(shaderProgram);
    glUseProgram(shaderProgram->value);
    POMDOG_CHECK_ERROR_GL4("glUseProgram");

    for (auto& binding : textureBindings) {
        glUniform1i(binding.UniformLocation, binding.SlotIndex);
        POMDOG_CHECK_ERROR_GL4("glUniform1i");
    }
}

ShaderProgramGL4 PipelineStateGL4::GetShaderProgram() const noexcept
{
    POMDOG_ASSERT(shaderProgram);
    return *shaderProgram;
}

InputLayoutGL4* PipelineStateGL4::GetInputLayout() const noexcept
{
    POMDOG_ASSERT(inputLayout);
    return inputLayout.get();
}

PrimitiveTopologyGL4 PipelineStateGL4::GetPrimitiveTopology() const noexcept
{
    return primitiveTopology;
}

} // namespace pomdog::detail::gl4
