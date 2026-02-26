// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::vulkan {

class ShaderVulkan final : public Shader {
public:
    ShaderVulkan(
        ::VkDevice device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);

    ~ShaderVulkan();

    [[nodiscard]] ::VkShaderModule
    getShaderModule() const noexcept;

private:
    ::VkDevice device;
    ::VkShaderModule shaderModule;
};

} // namespace pomdog::gpu::detail::vulkan
