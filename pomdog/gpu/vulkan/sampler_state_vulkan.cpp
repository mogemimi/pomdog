// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/sampler_state_vulkan.h"
#include "pomdog/gpu/sampler_desc.h"
#include "pomdog/gpu/vulkan/format_helper.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] VkSamplerAddressMode
toSamplerAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case TextureAddressMode::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case TextureAddressMode::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case TextureAddressMode::Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

[[nodiscard]] std::tuple<VkFilter, VkFilter, VkSamplerMipmapMode>
toTextureFilter(TextureFilter filter)
{
    switch (filter) {
    case TextureFilter::Anisotropic:
        return std::make_tuple(
            VK_FILTER_LINEAR,
            VK_FILTER_LINEAR,
            VK_SAMPLER_MIPMAP_MODE_LINEAR);
    case TextureFilter::Linear:
        return std::make_tuple(
            VK_FILTER_LINEAR,
            VK_FILTER_LINEAR,
            VK_SAMPLER_MIPMAP_MODE_LINEAR);
    case TextureFilter::Point:
        return std::make_tuple(
            VK_FILTER_NEAREST,
            VK_FILTER_NEAREST,
            VK_SAMPLER_MIPMAP_MODE_NEAREST);
    case TextureFilter::LinearMipPoint:
        return std::make_tuple(
            VK_FILTER_LINEAR,
            VK_FILTER_LINEAR,
            VK_SAMPLER_MIPMAP_MODE_NEAREST);
    case TextureFilter::PointMipLinear:
        return std::make_tuple(
            VK_FILTER_NEAREST,
            VK_FILTER_NEAREST,
            VK_SAMPLER_MIPMAP_MODE_LINEAR);
    case TextureFilter::MinLinearMagPointMipLinear:
        return std::make_tuple(
            VK_FILTER_LINEAR,
            VK_FILTER_NEAREST,
            VK_SAMPLER_MIPMAP_MODE_LINEAR);
    case TextureFilter::MinLinearMagPointMipPoint:
        return std::make_tuple(
            VK_FILTER_LINEAR,
            VK_FILTER_NEAREST,
            VK_SAMPLER_MIPMAP_MODE_NEAREST);
    case TextureFilter::MinPointMagLinearMipLinear:
        return std::make_tuple(
            VK_FILTER_NEAREST,
            VK_FILTER_LINEAR,
            VK_SAMPLER_MIPMAP_MODE_LINEAR);
    case TextureFilter::MinPointMagLinearMipPoint:
        return std::make_tuple(
            VK_FILTER_NEAREST,
            VK_FILTER_LINEAR,
            VK_SAMPLER_MIPMAP_MODE_NEAREST);
    }
    return std::make_tuple(
        VK_FILTER_NEAREST,
        VK_FILTER_NEAREST,
        VK_SAMPLER_MIPMAP_MODE_NEAREST);
}

} // namespace

std::unique_ptr<Error>
SamplerStateVulkan::initialize(
    ::VkDevice deviceIn,
    const SamplerDesc& descriptor) noexcept
{
    POMDOG_ASSERT(deviceIn != nullptr);

    device_ = deviceIn;
    sampler_ = nullptr;

    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    std::tie(
        createInfo.minFilter,
        createInfo.magFilter,
        createInfo.mipmapMode) = toTextureFilter(descriptor.filter);
    createInfo.addressModeU = toSamplerAddressMode(descriptor.addressU);
    createInfo.addressModeV = toSamplerAddressMode(descriptor.addressV);
    createInfo.addressModeW = toSamplerAddressMode(descriptor.addressW);
    createInfo.mipLodBias = 0.0f;
    createInfo.anisotropyEnable = VK_FALSE;

    if (descriptor.comparisonFunction == ComparisonFunction::Never) {
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_NEVER;
    }
    else {
        createInfo.compareEnable = VK_TRUE;
        createInfo.compareOp = toComparisonFunction(descriptor.comparisonFunction);
    }

    createInfo.minLod = descriptor.minMipLevel;
    createInfo.maxLod = descriptor.maxMipLevel;
    createInfo.maxAnisotropy = static_cast<f32>(descriptor.maxAnisotropy);
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    auto result = vkCreateSampler(device_, &createInfo, nullptr, &sampler_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkSampler");
    }

    return nullptr;
}

SamplerStateVulkan::~SamplerStateVulkan()
{
    if (sampler_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroySampler(device_, sampler_, nullptr);
    }
}

::VkSampler
SamplerStateVulkan::getSamplerState() const noexcept
{
    return sampler_;
}

} // namespace pomdog::gpu::detail::vulkan
