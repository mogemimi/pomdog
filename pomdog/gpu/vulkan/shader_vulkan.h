// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/shader.h"
#include <vulkan/vulkan.h>

namespace pomdog::detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace pomdog::detail

namespace pomdog::detail::vulkan {

class ShaderVulkan final : public Shader {
public:
    ShaderVulkan(
        ::VkDevice device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);

    ~ShaderVulkan();

    ::VkShaderModule GetShaderModule() const noexcept;

private:
    ::VkDevice device;
    ::VkShaderModule shaderModule;
};

} // namespace pomdog::detail::vulkan
