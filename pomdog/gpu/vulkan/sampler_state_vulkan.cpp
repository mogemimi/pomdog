// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/sampler_state_vulkan.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/gpu/vulkan/format_helper.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"
#include <tuple>

namespace pomdog::detail::vulkan {
namespace {

VkSamplerAddressMode ToSamplerAddressMode(TextureAddressMode addressMode) noexcept
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

std::tuple<VkFilter, VkFilter, VkSamplerMipmapMode>
ToTextureFilter(TextureFilter filter)
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

SamplerStateVulkan::SamplerStateVulkan(
    ::VkDevice deviceIn,
    const SamplerDescriptor& descriptor)
    : device(deviceIn)
    , sampler(nullptr)
{
    POMDOG_ASSERT(device != nullptr);

    VkSamplerCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    std::tie(
        createInfo.minFilter,
        createInfo.magFilter,
        createInfo.mipmapMode) = ToTextureFilter(descriptor.Filter);
    createInfo.addressModeU = ToSamplerAddressMode(descriptor.AddressU);
    createInfo.addressModeV = ToSamplerAddressMode(descriptor.AddressV);
    createInfo.addressModeW = ToSamplerAddressMode(descriptor.AddressW);
    createInfo.mipLodBias = 0.0f;
    createInfo.anisotropyEnable = VK_FALSE;

    if (descriptor.ComparisonFunction == ComparisonFunction::Never) {
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_NEVER;
    }
    else {
        createInfo.compareEnable = VK_TRUE;
        createInfo.compareOp = ToComparisonFunction(descriptor.ComparisonFunction);
    }

    createInfo.minLod = descriptor.MinMipLevel;
    createInfo.maxLod = descriptor.MaxMipLevel;
    createInfo.maxAnisotropy = descriptor.MaxAnisotropy;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    auto result = vkCreateSampler(device, &createInfo, nullptr, &sampler);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkSampler");
    }
}

SamplerStateVulkan::~SamplerStateVulkan()
{
    if (sampler != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroySampler(device, sampler, nullptr);
    }
}

::VkSampler SamplerStateVulkan::GetSamplerState() const
{
    return sampler;
}

} // namespace pomdog::detail::vulkan
