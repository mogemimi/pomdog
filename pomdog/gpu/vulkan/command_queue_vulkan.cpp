// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/command_queue_vulkan.h"
#include "pomdog/gpu/vulkan/command_list_vulkan.h"
#include "pomdog/gpu/vulkan/swap_chain_vulkan.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

namespace pomdog::gpu::detail::vulkan {

CommandQueueVulkan::CommandQueueVulkan(
    ::VkDevice deviceIn,
    ::VkQueue graphicsQueueIn)
    : device_(deviceIn)
    , graphicsQueue_(graphicsQueueIn)
    , submitFence_(nullptr)
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(graphicsQueue_ != nullptr);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;

    auto result = vkCreateFence(device_, &fenceInfo, nullptr, &submitFence_);
    if (result != VK_SUCCESS) {
        submitFence_ = nullptr;
    }
}

CommandQueueVulkan::~CommandQueueVulkan()
{
    if (submitFence_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyFence(device_, submitFence_, nullptr);
    }
}

void CommandQueueVulkan::reset()
{
    commandLists_.clear();
}

void CommandQueueVulkan::pushBackCommandList(
    const std::shared_ptr<CommandList>& commandList)
{
    POMDOG_ASSERT(commandList != nullptr);

    auto vulkanCommandList = std::dynamic_pointer_cast<CommandListVulkan>(commandList);
    POMDOG_ASSERT(vulkanCommandList != nullptr);

    commandLists_.push_back(std::move(vulkanCommandList));
}

void CommandQueueVulkan::executeCommandLists()
{
    POMDOG_ASSERT(graphicsQueue_ != nullptr);

    if (commandLists_.empty()) {
        return;
    }

    if (fatalError_ != nullptr) {
        commandLists_.clear();
        return;
    }

    // NOTE: Drop the frame while the swap chain is out of date or unusable.
    // Command lists skip render passes and draws after image acquisition fails.
    // The host recreates an out-of-date swap chain at the end of the frame.
    if ((swapChain_ != nullptr) && (swapChain_->isOutOfDate() || (swapChain_->getFatalError() != nullptr))) {
        commandLists_.clear();
        return;
    }

    // NOTE: Collect VkCommandBuffers from all command lists
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.reserve(commandLists_.size());

    for (auto& cmdList : commandLists_) {
        POMDOG_ASSERT(cmdList != nullptr);
        auto cb = cmdList->getCommandBuffer();
        if (cb != nullptr) {
            commandBuffers.push_back(cb);
        }
    }

    if (!commandBuffers.empty()) {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = static_cast<u32>(commandBuffers.size());
        submitInfo.pCommandBuffers = commandBuffers.data();

        // NOTE: Add swap chain synchronization semaphores
        VkSemaphore waitSemaphore;
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSemaphore signalSemaphore;

        if (swapChain_ != nullptr && swapChain_->isImageAcquired()) {
            waitSemaphore = swapChain_->getImageAvailableSemaphore();
            signalSemaphore = swapChain_->getRenderFinishedSemaphore();

            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &signalSemaphore;
        }

        // NOTE: Reset fence before submit
        if (submitFence_ != nullptr) {
            vkResetFences(device_, 1, &submitFence_);
        }

        if (auto result = vkQueueSubmit(graphicsQueue_, 1, &submitInfo, submitFence_); result != VK_SUCCESS) {
            fatalError_ = errors::make(pomdog::format(
                "vkQueueSubmit() failed with error code: {}", static_cast<i32>(result)));
            commandLists_.clear();
            return;
        }

        // NOTE: Wait for completion
        if (submitFence_ != nullptr) {
            if (auto result = vkWaitForFences(device_, 1, &submitFence_, VK_TRUE, UINT64_MAX); result != VK_SUCCESS) {
                fatalError_ = errors::make(pomdog::format(
                    "vkWaitForFences() failed with error code: {}", static_cast<i32>(result)));
                commandLists_.clear();
                return;
            }
        }
    }

    commandLists_.clear();
}

void CommandQueueVulkan::present()
{
    if (fatalError_ != nullptr) {
        return;
    }
    if (swapChain_ == nullptr || !swapChain_->isImageAcquired()) {
        return;
    }

    POMDOG_ASSERT(graphicsQueue_ != nullptr);

    VkSemaphore waitSemaphores[] = {swapChain_->getRenderFinishedSemaphore()};
    VkSwapchainKHR swapChains[] = {swapChain_->getSwapChain()};
    u32 imageIndex = swapChain_->getCurrentImageIndex();

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    const auto result = vkQueuePresentKHR(graphicsQueue_, &presentInfo);

    swapChain_->resetAcquiredState();

    // NOTE: VK_SUBOPTIMAL_KHR still presents the image, so it is treated as
    // success. VK_ERROR_OUT_OF_DATE_KHR means the surface changed, such as on
    // a resize, and the host recreates the swap chain at the end of the frame.
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain_->markOutOfDate();
    }
    else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR)) {
        fatalError_ = errors::make(pomdog::format(
            "vkQueuePresentKHR() failed with error code: {}", static_cast<i32>(result)));
    }
}

void CommandQueueVulkan::setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept
{
    swapChain_ = swapChain;
}

unsafe_ptr<const Error>
CommandQueueVulkan::getFatalError() const noexcept
{
    return fatalError_.get();
}

} // namespace pomdog::gpu::detail::vulkan
