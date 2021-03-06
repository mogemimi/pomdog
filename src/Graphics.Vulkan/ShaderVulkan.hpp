// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Vulkan {

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

} // namespace Pomdog::Detail::Vulkan
