// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "ShaderVulkan.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {

ShaderVulkan::ShaderVulkan(
    ::VkDevice deviceIn,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
    : device(deviceIn)
    , shaderModule(nullptr)
{
    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = shaderBytecode.ByteLength / sizeof(std::uint32_t);
    moduleCreateInfo.pCode = reinterpret_cast<const std::uint32_t*>(shaderBytecode.Code);

    auto result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkShaderModule");
    }

    ///@todo Not implemented

    // FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

ShaderVulkan::~ShaderVulkan()
{
    if (shaderModule != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
