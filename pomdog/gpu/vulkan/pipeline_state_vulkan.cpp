// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/pipeline_state_vulkan.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/vulkan/format_helper.h"
#include "pomdog/gpu/vulkan/shader_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] VkPrimitiveTopology
toVkPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
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

[[nodiscard]] VkVertexInputRate
toVkVertexInputRate(InputClassification classification) noexcept
{
    switch (classification) {
    case InputClassification::PerVertex:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case InputClassification::PerInstance:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

[[nodiscard]] VkFormat
toVkFormat(InputElementFormat format) noexcept
{
    switch (format) {
    case InputElementFormat::Float32x1:
        return VK_FORMAT_R32_SFLOAT;
    case InputElementFormat::Float32x2:
        return VK_FORMAT_R32G32_SFLOAT;
    case InputElementFormat::Float32x3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case InputElementFormat::Float32x4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case InputElementFormat::Uint8x1:
        return VK_FORMAT_R8_UINT;
    case InputElementFormat::Uint8x2:
        return VK_FORMAT_R8G8_UINT;
    case InputElementFormat::Uint8x4:
        return VK_FORMAT_R8G8B8A8_UINT;
    case InputElementFormat::Unorm8x1:
        return VK_FORMAT_R8_UNORM;
    case InputElementFormat::Unorm8x2:
        return VK_FORMAT_R8G8_UNORM;
    case InputElementFormat::Unorm8x4:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case InputElementFormat::Int32x1:
        return VK_FORMAT_R32_SINT;
    case InputElementFormat::Int32x2:
        return VK_FORMAT_R32G32_SINT;
    case InputElementFormat::Int32x3:
        return VK_FORMAT_R32G32B32_SINT;
    case InputElementFormat::Int32x4:
        return VK_FORMAT_R32G32B32A32_SINT;
    case InputElementFormat::Float16x2:
        return VK_FORMAT_R16G16_SFLOAT;
    case InputElementFormat::Float16x4:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    }
    return VK_FORMAT_R32G32B32A32_SFLOAT;
}

[[nodiscard]] VkBlendOp
toVkBlendOp(BlendOperation blendOperation) noexcept
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
    return VK_BLEND_OP_ADD;
}

[[nodiscard]] VkBlendFactor
toVkBlendFactor(BlendFactor blend) noexcept
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
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Source1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BlendFactor::InverseSource1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    case BlendFactor::BlendFactor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case BlendFactor::InvereseBlendFactor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    }
    return VK_BLEND_FACTOR_ONE;
}

[[nodiscard]] VkStencilOp
toStencilOperation(StencilOperation operation) noexcept
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

[[nodiscard]] VkCullModeFlags
toVkCullModeFlags(CullMode cullMode) noexcept
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

[[nodiscard]] VkPolygonMode
toVkPolygonMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::Solid:
        return VK_POLYGON_MODE_FILL;
    case FillMode::Wireframe:
        return VK_POLYGON_MODE_LINE;
    }
    return VK_POLYGON_MODE_FILL;
}

[[nodiscard]] VkBool32
toVkBool32(bool is) noexcept
{
    return is ? VK_TRUE : VK_FALSE;
}

[[nodiscard]] VkPipelineColorBlendAttachmentState
toColorBlendAttachmentState(const RenderTargetBlendDesc& desc) noexcept
{
    VkPipelineColorBlendAttachmentState state;
    state.blendEnable = toVkBool32(desc.blendEnable);
    state.srcColorBlendFactor = toVkBlendFactor(desc.colorSourceBlend);
    state.dstColorBlendFactor = toVkBlendFactor(desc.colorDestinationBlend);
    state.colorBlendOp = toVkBlendOp(desc.colorBlendOperation);
    state.srcAlphaBlendFactor = toVkBlendFactor(desc.alphaSourceBlend);
    state.dstAlphaBlendFactor = toVkBlendFactor(desc.alphaDestinationBlend);
    state.alphaBlendOp = toVkBlendOp(desc.alphaBlendOperation);
    state.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    return state;
}

void toBlendState(
    [[maybe_unused]] const BlendDesc& desc,
    const std::vector<VkPipelineColorBlendAttachmentState>& blendAttachments,
    VkPipelineColorBlendStateCreateInfo& state) noexcept
{
    state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    state.pNext = nullptr;
    state.flags = 0;
    state.logicOpEnable = VK_FALSE;
    state.logicOp = VK_LOGIC_OP_COPY;
    state.attachmentCount = static_cast<u32>(blendAttachments.size());
    state.pAttachments = blendAttachments.data();
    state.blendConstants[0] = 0.0f;
    state.blendConstants[1] = 0.0f;
    state.blendConstants[2] = 0.0f;
    state.blendConstants[3] = 0.0f;
}

void toDepthStencilOperation(
    const DepthStencilOperation& operation,
    const DepthStencilDesc& descriptor,
    VkStencilOpState& state) noexcept
{
    state.failOp = toStencilOperation(operation.stencilFail);
    state.passOp = toStencilOperation(operation.stencilPass);
    state.depthFailOp = toStencilOperation(operation.stencilDepthBufferFail);
    state.compareOp = toComparisonFunction(operation.stencilFunction);
    state.compareMask = descriptor.stencilMask;
    state.writeMask = descriptor.stencilWriteMask;
    state.reference = 0;
}

void toDepthStencilState(
    const DepthStencilDesc& desc,
    VkPipelineDepthStencilStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthTestEnable = toVkBool32(desc.depthBufferEnable);
    info.depthWriteEnable = toVkBool32(desc.depthBufferWriteEnable);
    info.depthCompareOp = desc.depthBufferEnable
                              ? toComparisonFunction(desc.depthBufferFunction)
                              : VK_COMPARE_OP_ALWAYS;
    info.stencilTestEnable = toVkBool32(desc.stencilEnable);
    toDepthStencilOperation(desc.clockwiseFace, desc, info.front);
    toDepthStencilOperation(desc.counterClockwiseFace, desc, info.back);

    // NOTE: Depth bounds test is disabled because DepthStencilDesc does not expose
    // depthBoundsEnable/minDepthBounds/maxDepthBounds fields yet.
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f;
    info.maxDepthBounds = 1.0f;
}

void toRasterizationState(
    const RasterizerDesc& desc,
    VkPipelineRasterizationStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthClampEnable = VK_FALSE;
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = toVkPolygonMode(desc.fillMode);
    info.cullMode = toVkCullModeFlags(desc.cullMode);
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0;
    info.depthBiasClamp = 0;
    info.depthBiasSlopeFactor = desc.slopeScaledDepthBias;
    info.lineWidth = 1.0f;
}

[[nodiscard]] std::tuple<std::vector<VkVertexInputBindingDescription>, std::vector<VkVertexInputAttributeDescription>>
toVertexBindingsAndAttributes(const InputLayoutDesc& inputLayout)
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    for (auto& bufferLayout : inputLayout.vertexBuffers) {
        // TODO: instanceStepRate is not wired up; needs
        // VK_EXT_vertex_attribute_divisor extension support.
        VkVertexInputBindingDescription bindingDesc = {};
        bindingDesc.binding = bufferLayout.inputSlot;
        bindingDesc.stride = bufferLayout.strideBytes;
        bindingDesc.inputRate = toVkVertexInputRate(bufferLayout.inputSlotClass);
        bindings.push_back(bindingDesc);

        for (auto& element : bufferLayout.elements) {
            VkVertexInputAttributeDescription attributeDesc;
            attributeDesc.binding = bufferLayout.inputSlot;
            attributeDesc.location = static_cast<u32>(attributes.size());
            attributeDesc.format = toVkFormat(element.format);
            attributeDesc.offset = element.byteOffset;
            attributes.push_back(std::move(attributeDesc));
        }
    }

    return std::make_tuple(std::move(bindings), std::move(attributes));
}

void toVertexInputState(
    [[maybe_unused]] const InputLayoutDesc& inputLayout,
    const std::vector<VkVertexInputBindingDescription>& vertexBindings,
    const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
    VkPipelineVertexInputStateCreateInfo& info) noexcept
{
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.vertexBindingDescriptionCount = static_cast<u32>(vertexBindings.size());
    info.pVertexBindingDescriptions = vertexBindings.data();
    info.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributes.size());
    info.pVertexAttributeDescriptions = vertexAttributes.data();
}

[[nodiscard]] std::unique_ptr<Error>
createDescriptorSetLayouts(
    VkDevice device,
    [[maybe_unused]] const PipelineDesc& descriptor,
    std::array<VkDescriptorSetLayout, PipelineStateVulkan::kDescriptorSetCount>& descriptorSetLayouts) noexcept
{
    // NOTE: Set [0]: Uniform buffer bindings (constant buffers)
    {
        std::array<VkDescriptorSetLayoutBinding, PipelineStateVulkan::kMaxConstantBufferSlots> bindings;
        std::array<VkDescriptorBindingFlags, PipelineStateVulkan::kMaxConstantBufferSlots> bindingFlags;
        for (u32 i = 0; i < PipelineStateVulkan::kMaxConstantBufferSlots; ++i) {
            auto& b = bindings[i];
            b = {};
            b.binding = i;
            b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            b.descriptorCount = 1;
            b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            b.pImmutableSamplers = nullptr;
            bindingFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo = {};
        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.bindingCount = static_cast<u32>(bindingFlags.size());
        flagsInfo.pBindingFlags = bindingFlags.data();

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = &flagsInfo;
        createInfo.bindingCount = static_cast<u32>(bindings.size());
        createInfo.pBindings = bindings.data();

        auto result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayouts[0]);
        if (result != VK_SUCCESS) {
            return errors::make("failed to create UBO descriptor set layout");
        }
    }

    // NOTE: Set [1]: Combined image sampler bindings (textures + samplers)
    {
        std::array<VkDescriptorSetLayoutBinding, PipelineStateVulkan::kMaxTextureSlots> bindings;
        std::array<VkDescriptorBindingFlags, PipelineStateVulkan::kMaxTextureSlots> bindingFlags;
        for (u32 i = 0; i < PipelineStateVulkan::kMaxTextureSlots; ++i) {
            auto& b = bindings[i];
            b = {};
            b.binding = i;
            b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            b.descriptorCount = 1;
            b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            b.pImmutableSamplers = nullptr;
            bindingFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo = {};
        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.bindingCount = static_cast<u32>(bindingFlags.size());
        flagsInfo.pBindingFlags = bindingFlags.data();

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = &flagsInfo;
        createInfo.bindingCount = static_cast<u32>(bindings.size());
        createInfo.pBindings = bindings.data();

        auto result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayouts[1]);
        if (result != VK_SUCCESS) {
            vkDestroyDescriptorSetLayout(device, descriptorSetLayouts[0], nullptr);
            descriptorSetLayouts[0] = nullptr;
            return errors::make("failed to create sampler descriptor set layout");
        }
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
createRenderPass(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    const PipelineDesc& descriptor,
    VkRenderPass& renderPass) noexcept
{
    std::array<VkAttachmentDescription, 9> colorAttachments;
    std::array<VkAttachmentReference, 9> colorAttachmentRefs;
    VkAttachmentReference* depthAttachmentRef = nullptr;

    POMDOG_ASSERT(colorAttachments.size() >= descriptor.renderTargetViewFormats.size());
    POMDOG_ASSERT(colorAttachmentRefs.size() >= descriptor.renderTargetViewFormats.size());

    u32 attachmentIndex = 0;

    for (const auto& surfaceFormat : descriptor.renderTargetViewFormats) {
        POMDOG_ASSERT(attachmentIndex < colorAttachments.size());
        POMDOG_ASSERT(attachmentIndex < colorAttachmentRefs.size());

        auto& colorAttachment = colorAttachments[attachmentIndex];
        colorAttachment.flags = 0;
        colorAttachment.format = toSurfaceFormat(surfaceFormat);
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        auto& colorAttachmentRef = colorAttachmentRefs[attachmentIndex];
        colorAttachmentRef.attachment = attachmentIndex;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        ++attachmentIndex;
    }
    const auto colorAttachmentCount = attachmentIndex;

    if (descriptor.depthStencilViewFormat != PixelFormat::Invalid) {
        POMDOG_ASSERT(attachmentIndex < colorAttachments.size());
        POMDOG_ASSERT(attachmentIndex < colorAttachmentRefs.size());

        auto& depthAttachment = colorAttachments[attachmentIndex];
        depthAttachment.flags = 0;
        depthAttachment.format = chooseDepthStencilFormat(physicalDevice, descriptor.depthStencilViewFormat);
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
    subpass.colorAttachmentCount = colorAttachmentCount;
    subpass.pColorAttachments = colorAttachmentRefs.data();
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = depthAttachmentRef;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    if (depthAttachmentRef != nullptr) {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    const auto totalAttachmentCount = colorAttachmentCount + (depthAttachmentRef != nullptr ? 1 : 0);

    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = totalAttachmentCount;
    createInfo.pAttachments = colorAttachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    auto result = vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create render pass");
    }

    return nullptr;
}

} // namespace

std::unique_ptr<Error>
PipelineStateVulkan::initialize(
    VkDevice deviceIn,
    VkPhysicalDevice physicalDevice,
    const PipelineDesc& descriptor) noexcept
{
    POMDOG_ASSERT(deviceIn != nullptr);

    device_ = deviceIn;
    pipeline_ = nullptr;
    pipelineLayout_ = nullptr;
    renderPass_ = nullptr;
    descriptorSetLayouts_ = {};

    auto vertexShaderVulkan = std::dynamic_pointer_cast<ShaderVulkan>(descriptor.vertexShader);
    if (!vertexShaderVulkan) {
        return errors::make("invalid vertex shader");
    }
    auto pixelShaderVulkan = std::dynamic_pointer_cast<ShaderVulkan>(descriptor.pixelShader);
    if (!pixelShaderVulkan) {
        return errors::make("invalid pixel shader");
    }

    auto vertexShaderModule = vertexShaderVulkan->getShaderModule();
    if (!vertexShaderModule) {
        return errors::make("the vertex shader module is null");
    }
    auto pixelShaderModule = pixelShaderVulkan->getShaderModule();
    if (!pixelShaderModule) {
        return errors::make("the pixel shader module is null");
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

#if defined(POMDOG_DEBUG_BUILD)
    if (auto err = gpu::InputLayoutBuilder::validate(descriptor.inputLayout); err != nullptr) {
        return errors::wrap(std::move(err), "input layout validation failed");
    }
#endif

    const auto [vertexBindings, vertexAttributes] = toVertexBindingsAndAttributes(descriptor.inputLayout);

    VkPipelineVertexInputStateCreateInfo vertexInputState;
    toVertexInputState(descriptor.inputLayout, vertexBindings, vertexAttributes, vertexInputState);

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = toVkPrimitiveTopology(descriptor.primitiveTopology);
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    toDepthStencilState(descriptor.depthStencilState, depthStencilState);

    VkPipelineRasterizationStateCreateInfo rasterizationState;
    toRasterizationState(descriptor.rasterizerState, rasterizationState);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
    {
        std::size_t index = 0;
        for (auto& renderTarget : descriptor.blendState.renderTargets) {
            if (index >= descriptor.renderTargetViewFormats.size()) {
                break;
            }
            blendAttachments.push_back(toColorBlendAttachmentState(renderTarget));
            ++index;
        }
    }

    VkPipelineColorBlendStateCreateInfo blendState;
    toBlendState(descriptor.blendState, blendAttachments, blendState);

    VkPipelineMultisampleStateCreateInfo multisampleState;
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.pNext = nullptr;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.minSampleShading = 0.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = toVkBool32(descriptor.blendState.alphaToCoverageEnable);
    multisampleState.alphaToOneEnable = VK_FALSE;

    if (auto err = createDescriptorSetLayouts(device_, descriptor, descriptorSetLayouts_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create descriptor set layouts");
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = static_cast<u32>(descriptorSetLayouts_.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts_.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    auto result = vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create pipeline layout");
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

    if (auto err = createRenderPass(device_, physicalDevice, descriptor, renderPass_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render pass");
    }

    std::array<VkDynamicState, 3> const dynamicStates = {{
        VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    }};

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = static_cast<u32>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = static_cast<u32>(shaderStages.size());
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
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass_;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    result = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create graphics pipeline");
    }

    return nullptr;
}

PipelineStateVulkan::~PipelineStateVulkan()
{
    if (pipeline_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyPipeline(device_, pipeline_, nullptr);
    }
    if (pipelineLayout_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    }
    for (auto& layout : descriptorSetLayouts_) {
        if (layout != nullptr) {
            POMDOG_ASSERT(device_ != nullptr);
            vkDestroyDescriptorSetLayout(device_, layout, nullptr);
        }
    }
    if (renderPass_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyRenderPass(device_, renderPass_, nullptr);
    }
}

VkPipeline
PipelineStateVulkan::getPipeline() const noexcept
{
    return pipeline_;
}

VkRenderPass
PipelineStateVulkan::getRenderPass() const noexcept
{
    return renderPass_;
}

VkPipelineLayout
PipelineStateVulkan::getPipelineLayout() const noexcept
{
    return pipelineLayout_;
}

const std::array<VkDescriptorSetLayout, PipelineStateVulkan::kDescriptorSetCount>&
PipelineStateVulkan::getDescriptorSetLayouts() const noexcept
{
    return descriptorSetLayouts_;
}

} // namespace pomdog::gpu::detail::vulkan
