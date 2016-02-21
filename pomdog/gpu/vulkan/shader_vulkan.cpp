// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/shader_vulkan.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::vulkan {

std::unique_ptr<Error>
ShaderVulkan::initialize(
    ::VkDevice deviceIn,
    std::span<const u8> shaderBytecode,
    [[maybe_unused]] const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(deviceIn != nullptr);

    device_ = deviceIn;
    shaderModule_ = nullptr;

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderBytecode.size();
    createInfo.pCode = reinterpret_cast<const u32*>(shaderBytecode.data());

    auto result = vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkShaderModule");
    }

    return nullptr;
}

ShaderVulkan::~ShaderVulkan()
{
    if (shaderModule_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyShaderModule(device_, shaderModule_, nullptr);
    }
}

::VkShaderModule
ShaderVulkan::getShaderModule() const noexcept
{
    return shaderModule_;
}

} // namespace pomdog::gpu::detail::vulkan
