// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/command_list_vulkan.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/gpu/vulkan/buffer_vulkan.h"
#include "pomdog/gpu/vulkan/depth_stencil_buffer_vulkan.h"
#include "pomdog/gpu/vulkan/pipeline_state_vulkan.h"
#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/gpu/vulkan/sampler_state_vulkan.h"
#include "pomdog/gpu/vulkan/swap_chain_vulkan.h"
#include "pomdog/gpu/vulkan/texture2d_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/math/color.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] VkIndexType
toVkIndexType(IndexFormat format) noexcept
{
    switch (format) {
    case IndexFormat::UInt16:
        return VK_INDEX_TYPE_UINT16;
    case IndexFormat::UInt32:
        return VK_INDEX_TYPE_UINT32;
    }
    return VK_INDEX_TYPE_UINT32;
}

} // namespace

CommandListVulkan::CommandListVulkan(
    ::VkDevice deviceIn,
    ::VkCommandPool commandPoolIn)
    : device_(deviceIn)
    , commandPool_(commandPoolIn)
    , commandBuffer_(nullptr)
    , descriptorPool_(nullptr)
    , indexType_(VK_INDEX_TYPE_UINT16)
    , isRecording_(false)
    , isInRenderPass_(false)
    , descriptorsDirty_(false)
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(commandPool_ != nullptr);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    auto result = vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer_);
    if (result != VK_SUCCESS) {
        // NOTE: Failed to allocate command buffer
        commandBuffer_ = nullptr;
    }

    // NOTE: Create a descriptor pool for per-frame descriptor set allocation.
    // Each draw call allocates kDescriptorSetCount (2) descriptor sets.
    // Complex GUI scenes (e.g., EditorGUITest) can easily exceed 64 draw calls
    // per frame due to scissor rect changes and texture switches, so we need
    // a large enough pool to avoid silent allocation failures.
    std::array<VkDescriptorPoolSize, 2> poolSizes = {{
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kMaxConstantBufferSlots * 512},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMaxTextureSlots * 512},
    }};

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets = 1024;
    poolCreateInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
    poolCreateInfo.pPoolSizes = poolSizes.data();

    result = vkCreateDescriptorPool(device_, &poolCreateInfo, nullptr, &descriptorPool_);
    if (result != VK_SUCCESS) {
        descriptorPool_ = nullptr;
    }
}

CommandListVulkan::~CommandListVulkan()
{
    for (auto fb : mrtFramebuffers_) {
        vkDestroyFramebuffer(device_, fb, nullptr);
    }
    for (auto rp : mrtRenderPasses_) {
        vkDestroyRenderPass(device_, rp, nullptr);
    }
    if (descriptorPool_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
    }
    if (commandBuffer_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        POMDOG_ASSERT(commandPool_ != nullptr);
        vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer_);
    }
}

VkCommandBuffer CommandListVulkan::getCommandBuffer() const noexcept
{
    return commandBuffer_;
}

void CommandListVulkan::close()
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);

    if (isInRenderPass_) {
        endRenderPass();
    }

    if (isRecording_) {
        vkEndCommandBuffer(commandBuffer_);
        isRecording_ = false;
    }
}

void CommandListVulkan::reset()
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);

    // NOTE: Destroy MRT resources from previous frame
    for (auto fb : mrtFramebuffers_) {
        vkDestroyFramebuffer(device_, fb, nullptr);
    }
    mrtFramebuffers_.clear();
    for (auto rp : mrtRenderPasses_) {
        vkDestroyRenderPass(device_, rp, nullptr);
    }
    mrtRenderPasses_.clear();

    vkResetCommandBuffer(commandBuffer_, 0);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer_, &beginInfo);
    isRecording_ = true;
    isInRenderPass_ = false;

    // NOTE: Reset descriptor pool for this frame
    if (descriptorPool_ != nullptr) {
        vkResetDescriptorPool(device_, descriptorPool_, 0);
    }

    clearBoundResources();
}

void CommandListVulkan::draw(
    u32 vertexCount,
    u32 startVertexLocation)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    flushDescriptorSets();

    vkCmdDraw(
        commandBuffer_,
        vertexCount,
        1, // instanceCount
        startVertexLocation,
        0); // firstInstance
}

void CommandListVulkan::drawIndexed(
    u32 indexCount,
    u32 startIndexLocation)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    flushDescriptorSets();

    vkCmdDrawIndexed(
        commandBuffer_,
        indexCount,
        1, // instanceCount
        startIndexLocation,
        0,  // vertexOffset
        0); // firstInstance
}

void CommandListVulkan::drawInstanced(
    u32 vertexCountPerInstance,
    u32 instanceCount,
    u32 startVertexLocation,
    u32 startInstanceLocation)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    flushDescriptorSets();

    vkCmdDraw(
        commandBuffer_,
        vertexCountPerInstance,
        instanceCount,
        startVertexLocation,
        startInstanceLocation);
}

void CommandListVulkan::drawIndexedInstanced(
    u32 indexCountPerInstance,
    u32 instanceCount,
    u32 startIndexLocation,
    u32 startInstanceLocation)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    flushDescriptorSets();

    vkCmdDrawIndexed(
        commandBuffer_,
        indexCountPerInstance,
        instanceCount,
        startIndexLocation,
        0, // vertexOffset
        startInstanceLocation);
}

void CommandListVulkan::beginRenderPass(RenderPass&& renderPass)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    if (isInRenderPass_) {
        endRenderPass();
    }

    // NOTE: Count active render targets
    u32 activeRTCount = 0;
    for (u32 i = 0; i < renderPass.renderTargets.size(); ++i) {
        auto& [rt, cc] = renderPass.renderTargets[i];
        if (rt != nullptr) {
            ++activeRTCount;
        }
        else {
            break;
        }
    }

    auto& [renderTarget, clearColor] = renderPass.renderTargets[0];

    if (activeRTCount > 1) {
        // NOTE: Multiple render targets (MRT)
        auto* rt0 = dynamic_cast<RenderTarget2DVulkan*>(renderTarget.get());
        POMDOG_ASSERT(rt0 != nullptr);

        // NOTE: Build attachment descriptions and references
        std::array<VkAttachmentDescription, 9> attachments = {};
        std::array<VkAttachmentReference, 8> colorRefs = {};
        u32 totalAttachments = 0;

        for (u32 i = 0; i < activeRTCount; ++i) {
            auto& [rt, cc] = renderPass.renderTargets[i];
            auto* rtVk = dynamic_cast<RenderTarget2DVulkan*>(rt.get());
            POMDOG_ASSERT(rtVk != nullptr);

            auto& att = attachments[totalAttachments];
            att.format = toSurfaceFormat(rtVk->getFormat());
            att.samples = VK_SAMPLE_COUNT_1_BIT;
            att.loadOp = cc.has_value() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            att.initialLayout = cc.has_value()
                                    ? VK_IMAGE_LAYOUT_UNDEFINED
                                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            att.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            colorRefs[i].attachment = totalAttachments;
            colorRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            ++totalAttachments;
        }

        // NOTE: Depth attachment (from external depth stencil buffer)
        VkAttachmentReference depthRef = {};
        VkAttachmentReference* pDepthRef = nullptr;
        VkImageView depthImageView = nullptr;

        if (renderPass.depthStencilBuffer != nullptr) {
            auto* dsb = dynamic_cast<DepthStencilBufferVulkan*>(renderPass.depthStencilBuffer.get());
            if (dsb != nullptr && dsb->getVkFormat() != VK_FORMAT_UNDEFINED) {
                depthImageView = dsb->getImageView();

                auto& depthAtt = attachments[totalAttachments];
                depthAtt.format = dsb->getVkFormat();
                depthAtt.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAtt.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAtt.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAtt.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAtt.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                depthRef.attachment = totalAttachments;
                depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                pDepthRef = &depthRef;
                ++totalAttachments;
            }
        }

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = activeRTCount;
        subpass.pColorAttachments = colorRefs.data();
        subpass.pDepthStencilAttachment = pDepthRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        if (pDepthRef != nullptr) {
            dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        VkRenderPassCreateInfo rpInfo = {};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpInfo.attachmentCount = totalAttachments;
        rpInfo.pAttachments = attachments.data();
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;
        rpInfo.dependencyCount = 1;
        rpInfo.pDependencies = &dependency;

        VkRenderPass mrtRenderPass = nullptr;
        if (vkCreateRenderPass(device_, &rpInfo, nullptr, &mrtRenderPass) != VK_SUCCESS) {
            return;
        }
        mrtRenderPasses_.push_back(mrtRenderPass);

        // NOTE: Create framebuffer with all image views
        std::array<VkImageView, 9> fbAttachments = {};
        u32 fbAttachmentCount = 0;

        for (u32 i = 0; i < activeRTCount; ++i) {
            auto& [rt, cc] = renderPass.renderTargets[i];
            auto* rtVk = dynamic_cast<RenderTarget2DVulkan*>(rt.get());
            fbAttachments[fbAttachmentCount++] = rtVk->getImageView();
        }
        if (depthImageView != nullptr) {
            fbAttachments[fbAttachmentCount++] = depthImageView;
        }

        VkFramebufferCreateInfo fbInfo = {};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = mrtRenderPass;
        fbInfo.attachmentCount = fbAttachmentCount;
        fbInfo.pAttachments = fbAttachments.data();
        fbInfo.width = static_cast<u32>(rt0->getWidth());
        fbInfo.height = static_cast<u32>(rt0->getHeight());
        fbInfo.layers = 1;

        VkFramebuffer mrtFramebuffer = nullptr;
        if (vkCreateFramebuffer(device_, &fbInfo, nullptr, &mrtFramebuffer) != VK_SUCCESS) {
            return;
        }
        mrtFramebuffers_.push_back(mrtFramebuffer);

        // Begin render pass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = mrtRenderPass;
        renderPassInfo.framebuffer = mrtFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.width = static_cast<u32>(rt0->getWidth());
        renderPassInfo.renderArea.extent.height = static_cast<u32>(rt0->getHeight());

        // NOTE: Clear values: one per color attachment + optional depth
        std::array<VkClearValue, 9> clearValues = {};
        for (u32 i = 0; i < activeRTCount; ++i) {
            auto& [rt, cc] = renderPass.renderTargets[i];
            if (cc.has_value()) {
                clearValues[i].color.float32[0] = cc->x;
                clearValues[i].color.float32[1] = cc->y;
                clearValues[i].color.float32[2] = cc->z;
                clearValues[i].color.float32[3] = cc->w;
            }
        }
        if (pDepthRef != nullptr) {
            clearValues[activeRTCount].depthStencil.depth = renderPass.clearDepth.value_or(1.0f);
            clearValues[activeRTCount].depthStencil.stencil = renderPass.clearStencil.value_or(0);
        }
        renderPassInfo.clearValueCount = totalAttachments;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        isInRenderPass_ = true;
    }
    else if (renderTarget != nullptr) {
        // NOTE: Off-screen render target
        auto* rt = dynamic_cast<RenderTarget2DVulkan*>(renderTarget.get());
        POMDOG_ASSERT(rt != nullptr);

        const auto rpHandle = clearColor.has_value()
                                  ? rt->getClearRenderPass()
                                  : rt->getLoadRenderPass();

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = rpHandle;
        renderPassInfo.framebuffer = rt->getFramebuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.width = static_cast<u32>(rt->getWidth());
        renderPassInfo.renderArea.extent.height = static_cast<u32>(rt->getHeight());

        std::array<VkClearValue, 2> clearValues = {};
        if (clearColor.has_value()) {
            clearValues[0].color.float32[0] = clearColor->x;
            clearValues[0].color.float32[1] = clearColor->y;
            clearValues[0].color.float32[2] = clearColor->z;
            clearValues[0].color.float32[3] = clearColor->w;
        }
        clearValues[1].depthStencil.depth = renderPass.clearDepth.value_or(1.0f);
        clearValues[1].depthStencil.stencil = renderPass.clearStencil.value_or(0);
        renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        isInRenderPass_ = true;
    }
    else if (swapChain_ != nullptr) {
        // NOTE: Default backbuffer via swap chain
        [[maybe_unused]] auto acquireResult = swapChain_->acquireNextImage();

        const auto rpHandle = swapChain_->selectRenderPass(clearColor.has_value());

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = rpHandle;
        renderPassInfo.framebuffer = swapChain_->getCurrentFramebuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain_->getExtent();

        std::array<VkClearValue, 2> clearValues = {};
        if (clearColor.has_value()) {
            clearValues[0].color.float32[0] = clearColor->x;
            clearValues[0].color.float32[1] = clearColor->y;
            clearValues[0].color.float32[2] = clearColor->z;
            clearValues[0].color.float32[3] = clearColor->w;
        }
        clearValues[1].depthStencil.depth = renderPass.clearDepth.value_or(1.0f);
        clearValues[1].depthStencil.stencil = renderPass.clearStencil.value_or(0);
        renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        isInRenderPass_ = true;
    }

    if (renderPass.viewport) {
        const auto& vp = *renderPass.viewport;
        if (vp.width > 0 && vp.height > 0) {
            setViewport(vp);
        }
    }
    if (renderPass.scissorRect) {
        const auto& sr = *renderPass.scissorRect;
        if (sr.width > 0 && sr.height > 0) {
            setScissorRect(sr);
        }
    }
}

void CommandListVulkan::endRenderPass()
{
    if (isInRenderPass_) {
        vkCmdEndRenderPass(commandBuffer_);
        isInRenderPass_ = false;
    }
}

void CommandListVulkan::setViewport(const Viewport& viewportIn)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    VkViewport viewport = {};
    viewport.x = static_cast<f32>(viewportIn.topLeftX);
    viewport.y = static_cast<f32>(viewportIn.topLeftY + viewportIn.height);
    viewport.width = static_cast<f32>(viewportIn.width);
    viewport.height = -static_cast<f32>(viewportIn.height);
    viewport.minDepth = viewportIn.minDepth;
    viewport.maxDepth = viewportIn.maxDepth;

    vkCmdSetViewport(commandBuffer_, 0, 1, &viewport);
}

void CommandListVulkan::setScissorRect(const Rect2D& rect)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    VkRect2D scissor = {};
    scissor.offset.x = rect.x;
    scissor.offset.y = rect.y;
    scissor.extent.width = static_cast<u32>(rect.width);
    scissor.extent.height = static_cast<u32>(rect.height);

    vkCmdSetScissor(commandBuffer_, 0, 1, &scissor);
}

void CommandListVulkan::setBlendFactor(const Vector4& blendFactor)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);

    const f32 constants[4] = {
        blendFactor.x,
        blendFactor.y,
        blendFactor.z,
        blendFactor.w,
    };

    vkCmdSetBlendConstants(commandBuffer_, constants);
}

void CommandListVulkan::setVertexBuffer(
    u32 index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    setVertexBuffer(index, vertexBuffer, 0);
}

void CommandListVulkan::setVertexBuffer(
    u32 index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    u32 offset)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);
    POMDOG_ASSERT(vertexBuffer != nullptr);

    auto nativeBuffer = dynamic_cast<BufferVulkan*>(vertexBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);

    VkBuffer buffers[] = {nativeBuffer->getBuffer()};
    VkDeviceSize offsets[] = {static_cast<VkDeviceSize>(offset)};

    vkCmdBindVertexBuffers(
        commandBuffer_,
        index,
        1,
        buffers,
        offsets);
}

void CommandListVulkan::setIndexBuffer(
    const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);
    POMDOG_ASSERT(indexBuffer != nullptr);

    auto nativeBuffer = dynamic_cast<BufferVulkan*>(indexBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);

    indexType_ = toVkIndexType(indexBuffer->getElementSize());

    vkCmdBindIndexBuffer(
        commandBuffer_,
        nativeBuffer->getBuffer(),
        0, // offset
        indexType_);
}

void CommandListVulkan::setPipelineState(
    const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);
    POMDOG_ASSERT(pipelineState != nullptr);

    pipelineState_ = std::dynamic_pointer_cast<PipelineStateVulkan>(pipelineState);
    POMDOG_ASSERT(pipelineState_ != nullptr);

    vkCmdBindPipeline(
        commandBuffer_,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineState_->getPipeline());

    descriptorsDirty_ = true;
}

void CommandListVulkan::setConstantBuffer(
    u32 index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);
    POMDOG_ASSERT(constantBuffer != nullptr);
    POMDOG_ASSERT(index < kMaxConstantBufferSlots);

    auto* nativeBuffer = dynamic_cast<BufferVulkan*>(constantBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);

    boundConstantBuffers_[index] = BoundBuffer{
        nativeBuffer->getBuffer(),
        0,
        static_cast<VkDeviceSize>(constantBuffer->getSizeInBytes()),
    };
    descriptorsDirty_ = true;
}

void CommandListVulkan::setConstantBuffer(
    u32 index,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
    u32 offset)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(isRecording_);
    POMDOG_ASSERT(constantBuffer != nullptr);
    POMDOG_ASSERT(index < kMaxConstantBufferSlots);

    auto* nativeBuffer = dynamic_cast<BufferVulkan*>(constantBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);

    const auto bufferSize = static_cast<VkDeviceSize>(constantBuffer->getSizeInBytes());
    const auto byteOffset = static_cast<VkDeviceSize>(offset);

    boundConstantBuffers_[index] = BoundBuffer{
        nativeBuffer->getBuffer(),
        byteOffset,
        bufferSize - byteOffset,
    };
    descriptorsDirty_ = true;
}

void CommandListVulkan::setTexture(u32 index)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(index < kMaxTextureSlots);

    boundTextures_[index] = nullptr;
    descriptorsDirty_ = true;
}

void CommandListVulkan::setTexture(
    u32 index,
    const std::shared_ptr<gpu::Texture>& texture)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(index < kMaxTextureSlots);

    if (auto* tex2d = dynamic_cast<Texture2DVulkan*>(texture.get())) {
        boundTextures_[index] = tex2d->getImageView();
    }
    else if (auto* rt = dynamic_cast<RenderTarget2DVulkan*>(texture.get())) {
        boundTextures_[index] = rt->getImageView();
    }
    descriptorsDirty_ = true;
}

void CommandListVulkan::setSamplerState(
    u32 index,
    const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(samplerState != nullptr);
    POMDOG_ASSERT(index < kMaxTextureSlots);

    auto* sampler = dynamic_cast<SamplerStateVulkan*>(samplerState.get());
    POMDOG_ASSERT(sampler != nullptr);

    boundSamplers_[index] = sampler->getSamplerState();
    descriptorsDirty_ = true;
}

void CommandListVulkan::flushDescriptorSets()
{
    if (!descriptorsDirty_ || pipelineState_ == nullptr || descriptorPool_ == nullptr) {
        return;
    }
    descriptorsDirty_ = false;

    const auto& setLayouts = pipelineState_->getDescriptorSetLayouts();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool_;
    allocInfo.descriptorSetCount = PipelineStateVulkan::kDescriptorSetCount;
    allocInfo.pSetLayouts = setLayouts.data();

    std::array<VkDescriptorSet, PipelineStateVulkan::kDescriptorSetCount> sets = {};
    auto result = vkAllocateDescriptorSets(device_, &allocInfo, sets.data());
    if (result != VK_SUCCESS) {
        return;
    }

    // Write descriptors
    std::array<VkDescriptorBufferInfo, kMaxConstantBufferSlots> bufferInfos = {};
    std::array<VkDescriptorImageInfo, kMaxTextureSlots> imageInfos = {};
    std::array<VkWriteDescriptorSet, kMaxConstantBufferSlots + kMaxTextureSlots> writes = {};
    u32 writeCount = 0;

    for (u32 i = 0; i < kMaxConstantBufferSlots; ++i) {
        if (boundConstantBuffers_[i].has_value()) {
            auto& info = bufferInfos[i];
            info.buffer = boundConstantBuffers_[i]->buffer;
            info.offset = boundConstantBuffers_[i]->offset;
            info.range = boundConstantBuffers_[i]->range;

            auto& w = writes[writeCount++];
            w = {};
            w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            w.dstSet = sets[0];
            w.dstBinding = i;
            w.descriptorCount = 1;
            w.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            w.pBufferInfo = &info;
        }
    }

    for (u32 i = 0; i < kMaxTextureSlots; ++i) {
        if (boundTextures_[i] != nullptr && boundSamplers_[i] != nullptr) {
            auto& info = imageInfos[i];
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.imageView = boundTextures_[i];
            info.sampler = boundSamplers_[i];

            auto& w = writes[writeCount++];
            w = {};
            w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            w.dstSet = sets[1];
            w.dstBinding = i;
            w.descriptorCount = 1;
            w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            w.pImageInfo = &info;
        }
    }

    if (writeCount > 0) {
        vkUpdateDescriptorSets(device_, writeCount, writes.data(), 0, nullptr);
    }

    vkCmdBindDescriptorSets(
        commandBuffer_,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineState_->getPipelineLayout(),
        0,
        static_cast<u32>(sets.size()),
        sets.data(),
        0, nullptr);
}

void CommandListVulkan::clearBoundResources()
{
    for (auto& cb : boundConstantBuffers_) {
        cb.reset();
    }
    boundTextures_.fill(nullptr);
    boundSamplers_.fill(nullptr);
    pipelineState_.reset();
    descriptorsDirty_ = false;
}

void CommandListVulkan::setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept
{
    swapChain_ = swapChain;
}

} // namespace pomdog::gpu::detail::vulkan
