// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail {
class ShaderBytecode;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Vulkan {

struct ShaderCompileOptions;

class ShaderVulkan final : public Shader {
public:
    ShaderVulkan(
        ::VkDevice device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);

    ~ShaderVulkan();

private:
//    VkPipelineShaderStageCreateInfo shaderStage;
    ::VkDevice device;
    ::VkShaderModule shaderModule;
};

} // namespace Pomdog::Detail::Vulkan
