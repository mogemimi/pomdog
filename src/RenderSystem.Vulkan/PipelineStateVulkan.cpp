// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "PipelineStateVulkan.hpp"
#include "../RenderSystem/BufferHelper.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Vulkan {
namespace {

VkVertexInputRate ToVkVertexInputRate(InputClassification classification) noexcept
{
    switch (classification) {
    case InputClassification::InputPerVertex:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case InputClassification::InputPerInstance:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
}

VkFormat ToVkFormat(InputElementFormat format) noexcept
{
    switch (format) {
    case InputElementFormat::Float:
        return VK_FORMAT_R32_SFLOAT;
    case InputElementFormat::Float2:
        return VK_FORMAT_R32G32_SFLOAT;
    case InputElementFormat::Float3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case InputElementFormat::Float4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case InputElementFormat::Int4:
        return VK_FORMAT_R32G32B32A32_SINT;
    case InputElementFormat::Byte4:
        return VK_FORMAT_R8G8B8A8_UINT;
    case InputElementFormat::HalfFloat2:
        return VK_FORMAT_R16G16_SFLOAT;
    case InputElementFormat::HalfFloat4:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    }
}

VkBlendOp ToVkBlendOp(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add:
        return VK_BLEND_OP_ADD;
    case BlendOperation::Subtract:
        return VK_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOperation::Min:
        return VK_BLEND_OP_MIN;
    case BlendOperation::Max:
        return VK_BLEND_OP_MAX;
    }
}

VkBlendFactor ToVkBlendFactor(Blend blend) noexcept
{
    switch (blend) {
    case Blend::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case Blend::One:
        return VK_BLEND_FACTOR_ONE;
    case Blend::SourceColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case Blend::InverseSourceColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case Blend::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case Blend::InverseSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case Blend::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case Blend::InverseDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case Blend::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case Blend::InverseDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case Blend::SourceAlphaSaturation:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case Blend::Source1Color:
        return VK_BLEND_FACTOR_SRC1_COLOR;
    case Blend::InverseSource1Color:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    case Blend::Source1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case Blend::InverseSource1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case Blend::BlendFactor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case Blend::InvereseBlendFactor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    }
    return VK_BLEND_FACTOR_ONE;
}

VkStencilOp ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep:
        return VK_STENCIL_OP_KEEP;
    case StencilOperation::Zero:
        return VK_STENCIL_OP_ZERO;
    case StencilOperation::Replace:
        return VK_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementSaturation:
        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOperation::DecrementSaturation:
        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOperation::Invert:
        return VK_STENCIL_OP_INVERT;
    case StencilOperation::Increment:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOperation::Decrement:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }
    return VK_STENCIL_OP_KEEP;
}

VkCompareOp ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never:
        return VK_COMPARE_OP_NEVER;
    case ComparisonFunction::Less:
        return VK_COMPARE_OP_LESS;
    case ComparisonFunction::Equal:
        return VK_COMPARE_OP_EQUAL;
    case ComparisonFunction::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case ComparisonFunction::Greater:
        return VK_COMPARE_OP_GREATER;
    case ComparisonFunction::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case ComparisonFunction::Always:
        return VK_COMPARE_OP_ALWAYS;
    }
    return VK_COMPARE_OP_LESS_OR_EQUAL;
}

VkCullModeFlags ToVkCullModeFlags(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace:
        return VK_CULL_MODE_FRONT_BIT;
    case CullMode::CounterClockwiseFace:
        return VK_CULL_MODE_BACK_BIT;
    case CullMode::None:
        return VK_CULL_MODE_NONE;
    }
    return VK_CULL_MODE_BACK_BIT;
}

VkPolygonMode ToVkPolygonMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame:
        return VK_POLYGON_MODE_LINE;
    case FillMode::Solid:
        return VK_POLYGON_MODE_FILL;
    }
    return VK_POLYGON_MODE_FILL;
}

VkBool32 ToVkBool32(bool is) noexcept
{
    return is ? VK_TRUE : VK_FALSE;
}

VkPipelineColorBlendAttachmentState ToColorBlendAttachmentState(const RenderTargetBlendDescription& desc) noexcept
{
    VkPipelineColorBlendAttachmentState state;
    state.blendEnable = ToVkBool32(desc.BlendEnable);
    state.srcColorBlendFactor = ToVkBlendFactor(desc.ColorSourceBlend);
    state.dstColorBlendFactor = ToVkBlendFactor(desc.ColorDestinationBlend);
    state.colorBlendOp = ToVkBlendOp(desc.ColorBlendOperation);
    state.srcAlphaBlendFactor = ToVkBlendFactor(desc.AlphaSourceBlend);
    state.dstAlphaBlendFactor = ToVkBlendFactor(desc.AlphaDestinationBlend);
    state.alphaBlendOp = ToVkBlendOp(desc.AlphaBlendOperation);
    state.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    return state;
}

void ToBlendState(
    const BlendDescription& desc,
    const std::vector<VkPipelineColorBlendAttachmentState>& blendAttachments,
    VkPipelineColorBlendStateCreateInfo& state) noexcept
{
    state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    state.pNext = nullptr;
    state.flags = 0;
    state.logicOpEnable = VK_FALSE;
    state.logicOp = VK_LOGIC_OP_COPY;
    state.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
    state.pAttachments = blendAttachments.data();
    state.blendConstants[0] = 0.0f;
    state.blendConstants[1] = 0.0f;
    state.blendConstants[2] = 0.0f;
    state.blendConstants[3] = 0.0f;
}

void ToDepthStencilOperation(
    const DepthStencilOperation& operation,
    const DepthStencilDescription& description,
    VkStencilOpState& state) noexcept
{
    state.failOp = ToStencilOperation(operation.StencilFail);
    state.passOp = ToStencilOperation(operation.StencilPass);
    state.depthFailOp = ToStencilOperation(operation.StencilDepthBufferFail);
    state.compareOp = ToComparisonFunction(operation.StencilFunction);
    state.compareMask = description.StencilMask;
    state.writeMask = description.StencilWriteMask;
    state.reference = 0;
}

void ToDepthStencilState(
    const DepthStencilDescription& desc,
    VkPipelineDepthStencilStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthTestEnable = ToVkBool32(desc.DepthBufferEnable);
    info.depthWriteEnable = ToVkBool32(desc.DepthBufferWriteEnable);
    info.depthCompareOp = desc.DepthBufferEnable
        ? ToComparisonFunction(desc.DepthBufferFunction)
        : VK_COMPARE_OP_ALWAYS;
    info.stencilTestEnable = ToVkBool32(desc.StencilEnable);
    ToDepthStencilOperation(desc.ClockwiseFace, desc, info.front);
    ToDepthStencilOperation(desc.CounterClockwiseFace, desc, info.back);

    // TODO: Not implemented
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f;
    info.maxDepthBounds = 1.0f;
}

void ToRasterizationState(
    const RasterizerDescription& desc,
    VkPipelineRasterizationStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthClampEnable = VK_FALSE;
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = ToVkPolygonMode(desc.FillMode);
    info.cullMode = ToVkCullModeFlags(desc.CullMode);
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0;
    info.depthBiasClamp = 0;
    info.depthBiasSlopeFactor = desc.SlopeScaledDepthBias;
    info.lineWidth = 1;
}

void ToVertexInputState(
    const InputLayoutDescription& inputLayout,
    const std::vector<VkVertexInputBindingDescription>& vertexBindings,
    const std::vector<VkVertexInputAttributeDescription> vertexAttributes,
    VkPipelineVertexInputStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
    info.pVertexBindingDescriptions = vertexBindings.data();
    info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    info.pVertexAttributeDescriptions = vertexAttributes.data();
}

} // unnamed namespace

PipelineStateVulkan::PipelineStateVulkan(
    VkDevice device,
    const PipelineStateDescription& description)
    : pipeline(nullptr)
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

    std::vector<VkVertexInputBindingDescription> vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    {
        for (auto& element : description.InputLayout.InputElements) {
            VkVertexInputBindingDescription inputBinding;
            inputBinding.binding = element.InputSlot; // TODO
            inputBinding.stride = element.ByteOffset + BufferHelper::ToByteSize(element.Format);
            inputBinding.inputRate = ToVkVertexInputRate(element.InputSlotClass);
            vertexBindings.push_back(std::move(inputBinding));

            VkVertexInputAttributeDescription inputAttribute;
            inputAttribute.location = element.InputSlot; // TODO
            inputAttribute.binding = element.InputSlot; // TODO
            inputAttribute.format = ToVkFormat(element.Format);
            inputAttribute.offset = element.ByteOffset;
            vertexAttributes.push_back(std::move(inputAttribute));
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputState;
    ToVertexInputState(description.InputLayout, vertexBindings, vertexAttributes, vertexInputState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    ToDepthStencilState(description.DepthStencilState, depthStencilState);

    VkPipelineRasterizationStateCreateInfo rasterizationState;
    ToRasterizationState(description.RasterizerState, rasterizationState);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
    {
        std::size_t index = 0;
        for (auto& renderTarget : description.BlendState.RenderTargets) {
            if (index >= description.RenderTargetViewFormats.size()) {
                break;
            }
            blendAttachments.push_back(ToColorBlendAttachmentState(renderTarget));
            ++index;
        }
    }

    VkPipelineColorBlendStateCreateInfo blendState;
    ToBlendState(description.BlendState, blendAttachments, blendState);

    ///@todo Not implemented

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    ///@todo Not implemented

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    ///@todo Not implemented

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    ///@todo Not implemented

    VkPipelineLayout pipelineLayout = nullptr;
    auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create pipeline layout");
    }

    ///@todo Not implemented
    VkRenderPass renderPass = nullptr;

    ///@todo Not implemented
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineMultisampleStateCreateInfo multisampleState;

    ///@todo Not implemented

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create pipeline state");
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Pomdog::Detail::Vulkan
