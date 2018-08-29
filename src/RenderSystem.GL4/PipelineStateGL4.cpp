// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "PipelineStateGL4.hpp"
#include "EffectReflectionGL4.hpp"
#include "ErrorChecker.hpp"
#include "InputLayoutGL4.hpp"
#include "ShaderGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>
#include <type_traits>
#include <unordered_set>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

// NOTE: Please refer to D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT.
static constexpr std::size_t ConstantBufferSlotCount = 14;

std::optional<ShaderProgramGL4> LinkShaders(
    const VertexShaderGL4& vertexShader, const PixelShaderGL4& pixelShader)
{
    ShaderProgramGL4 const program {
        glCreateProgram()
    };

    glAttachShader(program.value, vertexShader.GetShader());
    glAttachShader(program.value, pixelShader.GetShader());

    glLinkProgram(program.value);
    POMDOG_CHECK_ERROR_GL4("glLinkProgram");

    GLint linkSuccess = 0;
    glGetProgramiv(program.value, GL_LINK_STATUS, &linkSuccess);

    if (linkSuccess == GL_FALSE) {
#ifdef DEBUG
        std::array<GLchar, 256> messageBuffer;

        glGetProgramInfoLog(program.value, messageBuffer.size(), 0, messageBuffer.data());
        std::string const message = messageBuffer.data();

        Log::Critical("Pomdog.RenderSystem", StringHelper::Format(
            "Failed to link shaders.\nerror: %s", message.c_str()));
#endif // defined(DEBUG)

        glDeleteProgram(program.value);
        return std::nullopt;
    }

    return program;
}

} // unnamed namespace

PipelineStateGL4::PipelineStateGL4(const PipelineStateDescription& description)
    : blendState(description.BlendState)
    , rasterizerState(description.RasterizerState)
    , depthStencilState(description.DepthStencilState)
{
    auto vertexShader = std::dynamic_pointer_cast<VertexShaderGL4>(description.VertexShader);
    if (!vertexShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderGL4>(description.PixelShader);
    if (!pixelShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
    }

    POMDOG_ASSERT(vertexShader);
    POMDOG_ASSERT(pixelShader);

    shaderProgram = LinkShaders(*vertexShader, *pixelShader);

    if (!shaderProgram) {
        POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to link shader program.");
    }

    inputLayout = std::make_unique<InputLayoutGL4>(*shaderProgram, description.InputLayout);

    EffectReflectionGL4 shaderReflection(*shaderProgram);
    {
        auto uniformBlocks = shaderReflection.GetNativeUniformBlocks();

        std::unordered_set<int> reservedSlots;
        std::unordered_set<int> reservedBlocks;

        auto & bindSlots = description.ConstantBufferBindSlots;

        for (auto & uniformBlock : uniformBlocks) {
            auto binding = bindSlots.find(uniformBlock.Name);
            if (binding != std::end(bindSlots)) {
                auto slotIndex = binding->second;
                glUniformBlockBinding(shaderProgram->value, uniformBlock.BlockIndex, slotIndex);
                reservedSlots.insert(slotIndex);
                reservedBlocks.insert(uniformBlock.BlockIndex);
            }
        }

        GLuint slotIndex = 0;
        for (auto & uniformBlock : uniformBlocks) {
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
                break;
            }

            TextureBindingGL4 binding;
            binding.UniformLocation = uniform.Location;
            binding.SlotIndex = slotIndex;
            textureBindings.push_back(binding);
            ++slotIndex;
        }
    }
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

ShaderProgramGL4 PipelineStateGL4::GetShaderProgram() const
{
    POMDOG_ASSERT(shaderProgram);
    return *shaderProgram;
}

InputLayoutGL4* PipelineStateGL4::GetInputLayout() const
{
    POMDOG_ASSERT(inputLayout);
    return inputLayout.get();
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
