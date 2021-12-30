// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/pipeline_state_vulkan.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/input_layout_descriptor.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/vulkan/format_helper.h"
#include "pomdog/gpu/vulkan/shader_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::vulkan {
namespace {

VkPrimitiveTopology
ToVkPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PrimitiveTopology::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveTopology::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveTopology::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    }
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

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

VkBlendFactor ToVkBlendFactor(BlendFactor blend) noexcept
{
    switch (blend) {
    case BlendFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case BlendFactor::SourceColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::InverseSourceColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::InverseSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::InverseDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::InverseDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::SourceAlphaSaturation:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case BlendFactor::Source1Color:
        return VK_BLEND_FACTOR_SRC1_COLOR;
    case BlendFactor::InverseSource1Color:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    case BlendFactor::Source1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BlendFactor::InverseSource1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::BlendFactor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case BlendFactor::InvereseBlendFactor:
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
    case FillMode::Solid:
        return VK_POLYGON_MODE_FILL;
    case FillMode::Wireframe:
        return VK_POLYGON_MODE_LINE;
    }
    return VK_POLYGON_MODE_FILL;
}

VkBool32 ToVkBool32(bool is) noexcept
{
    return is ? VK_TRUE : VK_FALSE;
}

VkPipelineColorBlendAttachmentState ToColorBlendAttachmentState(const RenderTargetBlendDescriptor& desc) noexcept
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
    const BlendDescriptor& desc,
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
    const DepthStencilDescriptor& descriptor,
    VkStencilOpState& state) noexcept
{
    state.failOp = ToStencilOperation(operation.StencilFail);
    state.passOp = ToStencilOperation(operation.StencilPass);
    state.depthFailOp = ToStencilOperation(operation.StencilDepthBufferFail);
    state.compareOp = ToComparisonFunction(operation.StencilFunction);
    state.compareMask = descriptor.StencilMask;
    state.writeMask = descriptor.StencilWriteMask;
    state.reference = 0;
}

void ToDepthStencilState(
    const DepthStencilDescriptor& desc,
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
    const RasterizerDescriptor& desc,
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
    info.lineWidth = 1.0f;
}

std::tuple<std::vector<VkVertexInputBindingDescription>, std::vector<VkVertexInputAttributeDescription>>
ToVertexBindingsAndAttributes(const InputLayoutDescriptor& inputLayout)
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkVertexInputBindingDescription bindingDesc;
    bindingDesc.stride = 0;
    bool hasInputBinding = false;

    for (auto& element : inputLayout.InputElements) {
        if (hasInputBinding && (bindingDesc.binding != element.InputSlot)) {
            // NOTE: Flush
            bindings.push_back(std::move(bindingDesc));

            bindingDesc.stride = 0;
            hasInputBinding = false;
        }

        const auto stride = static_cast<std::uint32_t>(element.ByteOffset) + BufferHelper::ToByteSize(element.Format);
        bindingDesc.binding = element.InputSlot;
        bindingDesc.stride = std::max(bindingDesc.stride, stride);
        bindingDesc.inputRate = ToVkVertexInputRate(element.InputSlotClass);
        hasInputBinding = true;

        VkVertexInputAttributeDescription attributeDesc;
        attributeDesc.binding = element.InputSlot;
        attributeDesc.location = static_cast<std::uint32_t>(attributes.size());
        attributeDesc.format = ToVkFormat(element.Format);
        attributeDesc.offset = element.ByteOffset;
        attributes.push_back(std::move(attributeDesc));
    }

    if (hasInputBinding) {
        // NOTE: Flush
        bindings.push_back(std::move(bindingDesc));
    }
    return std::make_tuple(std::move(bindings), std::move(attributes));
}

void ToVertexInputState(
    const InputLayoutDescriptor& inputLayout,
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

void CreateDescriptorSetLayout(
    VkDevice device,
    const PipelineStateDescriptor& descriptor,
    VkDescriptorSetLayout& descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.reserve(descriptor.ConstantBufferBindSlots.size());

    for (const auto& slot : descriptor.ConstantBufferBindSlots) {
        VkDescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = static_cast<std::uint32_t>(slot.second);

        // FIXME:
        // layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBinding.pImmutableSamplers = nullptr;
    }

    std::stable_sort(
        std::begin(layoutBindings),
        std::end(layoutBindings),
        [](const auto& a, const auto& b) -> bool { return a.binding < b.binding; });

    VkDescriptorSetLayoutCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();

    auto result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create descriptor set layout");
    }
}

void CreateRenderPass(
    VkDevice device,
    const PipelineStateDescriptor& descriptor,
    VkRenderPass& renderPass)
{
    std::array<VkAttachmentDescription, 9> colorAttachments;
    std::array<VkAttachmentReference, 9> colorAttachmentRefs;
    VkAttachmentReference* depthAttachmentRef = nullptr;

    POMDOG_ASSERT(colorAttachments.size() >= descriptor.RenderTargetViewFormats.size());
    POMDOG_ASSERT(colorAttachmentRefs.size() >= descriptor.RenderTargetViewFormats.size());

    std::uint32_t attachmentIndex = 0;

    for (const auto& surfaceFormat : descriptor.RenderTargetViewFormats) {
        POMDOG_ASSERT(attachmentIndex < colorAttachments.size());
        POMDOG_ASSERT(attachmentIndex < colorAttachmentRefs.size());

        auto& colorAttachment = colorAttachments[attachmentIndex];
        colorAttachment.flags = 0;
        colorAttachment.format = ToSurfaceFormat(surfaceFormat);
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        auto colorAttachmentRef = colorAttachmentRefs[attachmentIndex];
        colorAttachmentRef.attachment = attachmentIndex;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        ++attachmentIndex;
    }
    const auto colorAttachemntCount = attachmentIndex;

    if (descriptor.DepthStencilViewFormat != SurfaceFormat::Invalid) {
        POMDOG_ASSERT(attachmentIndex < colorAttachments.size());
        POMDOG_ASSERT(attachmentIndex < colorAttachmentRefs.size());

        auto& depthAttachment = colorAttachments[attachmentIndex];
        depthAttachment.flags = 0;
        depthAttachment.format = ToSurfaceFormat(descriptor.DepthStencilViewFormat);
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        auto& attachmentRef = colorAttachmentRefs[attachmentIndex];
        attachmentRef.attachment = attachmentIndex;
        attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        depthAttachmentRef = &attachmentRef;
    }

    VkSubpassDescription subpass;
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = colorAttachemntCount;
    subpass.pColorAttachments = colorAttachmentRefs.data();
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = depthAttachmentRef;
    subpass.preserveAttachmentCount = 1;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = static_cast<std::uint32_t>(colorAttachments.size());
    createInfo.pAttachments = colorAttachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    auto result = vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create render pass");
    }
}

} // namespace

PipelineStateVulkan::PipelineStateVulkan(
    VkDevice deviceIn,
    const PipelineStateDescriptor& descriptor)
    : device(deviceIn)
    , pipeline(nullptr)
    , renderPass(nullptr)
    , descriptorSetLayout(nullptr)
{
    POMDOG_ASSERT(device != nullptr);

    auto vertexShaderVulkan = std::dynamic_pointer_cast<ShaderVulkan>(descriptor.VertexShader);
    if (!vertexShaderVulkan) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
    }
    auto pixelShaderVulkan = std::dynamic_pointer_cast<ShaderVulkan>(descriptor.PixelShader);
    if (!pixelShaderVulkan) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
    }

    auto vertexShaderModule = vertexShaderVulkan->GetShaderModule();
    if (!vertexShaderModule) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The vertex shader is null");
    }
    auto pixelShaderModule = pixelShaderVulkan->GetShaderModule();
    if (!pixelShaderModule) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The pixel shader is null");
    }

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo;
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.pNext = nullptr;
    vertexShaderStageInfo.flags = 0;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";
    vertexShaderStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo pixelShaderStageInfo;
    pixelShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pixelShaderStageInfo.pNext = nullptr;
    pixelShaderStageInfo.flags = 0;
    pixelShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pixelShaderStageInfo.module = pixelShaderModule;
    pixelShaderStageInfo.pName = "main";
    pixelShaderStageInfo.pSpecializationInfo = nullptr;

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {{
        vertexShaderStageInfo,
        pixelShaderStageInfo,
    }};

    const auto [vertexBindings, vertexAttributes] = ToVertexBindingsAndAttributes(descriptor.InputLayout);

    VkPipelineVertexInputStateCreateInfo vertexInputState;
    ToVertexInputState(descriptor.InputLayout, vertexBindings, vertexAttributes, vertexInputState);

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = ToVkPrimitiveTopology(descriptor.PrimitiveTopology);
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    ToDepthStencilState(descriptor.DepthStencilState, depthStencilState);

    VkPipelineRasterizationStateCreateInfo rasterizationState;
    ToRasterizationState(descriptor.RasterizerState, rasterizationState);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
    {
        std::size_t index = 0;
        for (auto& renderTarget : descriptor.BlendState.RenderTargets) {
            if (index >= descriptor.RenderTargetViewFormats.size()) {
                break;
            }
            blendAttachments.push_back(ToColorBlendAttachmentState(renderTarget));
            ++index;
        }
    }

    VkPipelineColorBlendStateCreateInfo blendState;
    ToBlendState(descriptor.BlendState, blendAttachments, blendState);

    VkPipelineMultisampleStateCreateInfo multisampleState;
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.pNext = nullptr;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.minSampleShading = 0.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = ToVkBool32(descriptor.BlendState.AlphaToCoverageEnable);
    multisampleState.alphaToOneEnable = VK_FALSE;

    CreateDescriptorSetLayout(device, descriptor, descriptorSetLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VkPipelineLayout pipelineLayout = nullptr;
    auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create pipeline layout");
    }

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 1.0f;
    viewport.height = 1.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = 1;
    scissor.extent.height = 1;

    VkPipelineViewportStateCreateInfo viewportState;
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    CreateRenderPass(device, descriptor, renderPass);

    std::array<VkDynamicState, 3> const dynamicStates = {{
        VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    }};

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = static_cast<std::uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create pipeline state");
    }
}

PipelineStateVulkan::~PipelineStateVulkan()
{
    if (pipeline != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyPipeline(device, pipeline, nullptr);
    }
    if (descriptorSetLayout != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }
    if (renderPass != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
    }
}

} // namespace pomdog::gpu::detail::vulkan
