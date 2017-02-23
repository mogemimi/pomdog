// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "PipelineStateVulkan.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

VkVertexInputRate ToVertexStepFunction(InputClassification classification) noexcept
{
    switch (classification) {
    case InputClassification::InputPerVertex:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case InputClassification::InputPerInstance:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
}

VkBlendOp ToVkBlendOp(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add: return VK_BLEND_OP_ADD;
    case BlendOperation::Subtract: return VK_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOperation::Min: return VK_BLEND_OP_MIN;
    case BlendOperation::Max: return VK_BLEND_OP_MAX;
    }
}

VkBlendFactor ToVkBlendFactor(Blend blend) noexcept
{
    switch (blend) {
    case Blend::Zero: return VK_BLEND_FACTOR_ZERO;
    case Blend::One: return VK_BLEND_FACTOR_ONE;
    case Blend::SourceColor: return VK_BLEND_FACTOR_SRC_COLOR;
    case Blend::InverseSourceColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case Blend::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
    case Blend::InverseSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case Blend::DestinationAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
    case Blend::InverseDestinationAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case Blend::DestinationColor: return VK_BLEND_FACTOR_DST_COLOR;
    case Blend::InverseDestinationColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case Blend::SourceAlphaSaturation: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case Blend::Source1Color: return VK_BLEND_FACTOR_SRC1_COLOR;
    case Blend::InverseSource1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    case Blend::Source1Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
    case Blend::InverseSource1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case Blend::BlendFactor: return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case Blend::InvereseBlendFactor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    }
    return VK_BLEND_FACTOR_ONE;
}

VkStencilOp ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep: return VK_STENCIL_OP_KEEP;
    case StencilOperation::Zero: return VK_STENCIL_OP_ZERO;
    case StencilOperation::Replace: return VK_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementSaturation: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOperation::DecrementSaturation: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOperation::Invert: return VK_STENCIL_OP_INVERT;
    case StencilOperation::Increment: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOperation::Decrement: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }
    return VK_STENCIL_OP_KEEP;
}

VkCompareOp ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never: return VK_COMPARE_OP_NEVER;
    case ComparisonFunction::Less:  return VK_COMPARE_OP_LESS;
    case ComparisonFunction::Equal: return VK_COMPARE_OP_EQUAL;
    case ComparisonFunction::LessEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
    case ComparisonFunction::Greater:   return VK_COMPARE_OP_GREATER;
    case ComparisonFunction::NotEqual:  return VK_COMPARE_OP_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case ComparisonFunction::Always: return VK_COMPARE_OP_ALWAYS;
    }
    return VK_COMPARE_OP_LESS_OR_EQUAL;
}

VkFrontFace ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace: return VK_FRONT_FACE_CLOCKWISE;
    case CullMode::CounterClockwiseFace: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case CullMode::None:
        // NOTE: Please see `VkStencilFaceFlagBits`
        break;
    }
    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

VkPolygonMode ToVkPolygonMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame: return VK_POLYGON_MODE_LINE;
    case FillMode::Solid: return VK_POLYGON_MODE_FILL;
    }
    return VK_POLYGON_MODE_FILL;
}

} // unnamed namespace

PipelineStateVulkan::PipelineStateVulkan(
    VkDevice device,
    const PipelineStateDescription& description)
{
    POMDOG_ASSERT(device != nullptr);

    VkPipelineShaderStageCreateInfo vertexShaderCreateInfo;
    vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderCreateInfo.pNext = nullptr;
    vertexShaderCreateInfo.flags = 0;
    vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo.module = nullptr;
    vertexShaderCreateInfo.pName = "main";
    vertexShaderCreateInfo.pSpecializationInfo = nullptr;

    ///@todo Not implemented

    VkPipelineShaderStageCreateInfo pixelShaderCreateInfo;
    pixelShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pixelShaderCreateInfo.pNext = nullptr;
    pixelShaderCreateInfo.flags = 0;
    pixelShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pixelShaderCreateInfo.module = nullptr;
    pixelShaderCreateInfo.pName = "main";
    pixelShaderCreateInfo.pSpecializationInfo = nullptr;

    ///@todo Not implemented

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
