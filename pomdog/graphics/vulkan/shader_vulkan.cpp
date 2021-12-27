// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vulkan/shader_vulkan.h"
#include "pomdog/graphics/backends/shader_bytecode.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::detail::vulkan {

ShaderVulkan::ShaderVulkan(
    ::VkDevice deviceIn,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
    : device(deviceIn)
    , shaderModule(nullptr)
{
    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = shaderBytecode.ByteLength / sizeof(std::uint32_t);
    createInfo.pCode = reinterpret_cast<const std::uint32_t*>(shaderBytecode.Code);

    auto result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkShaderModule");
    }
}

ShaderVulkan::~ShaderVulkan()
{
    if (shaderModule != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }
}

::VkShaderModule ShaderVulkan::GetShaderModule() const noexcept
{
    return shaderModule;
}

} // namespace pomdog::detail::vulkan
