// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
struct ShaderCompileOptions;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::vulkan {

class ShaderVulkan final : public Shader {
private:
    ::VkDevice device_ = nullptr;
    ::VkShaderModule shaderModule_ = nullptr;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        std::span<const u8> shaderBytecode,
        const ShaderCompileOptions& compileOptions) noexcept;

    ~ShaderVulkan();

    [[nodiscard]] ::VkShaderModule
    getShaderModule() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
