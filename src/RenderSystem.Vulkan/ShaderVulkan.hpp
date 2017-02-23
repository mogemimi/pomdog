// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;

namespace Vulkan {

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

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
