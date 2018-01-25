// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SamplerStateVulkan.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

VkSamplerAddressMode ToSamplerAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case TextureAddressMode::Clamp: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case TextureAddressMode::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
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

} // unnamed namespace

SamplerStateVulkan::SamplerStateVulkan(
    ::VkDevice deviceIn,
    const SamplerDescription& description)
    : device(deviceIn)
    , sampler(nullptr)
{
    POMDOG_ASSERT(device != nullptr);

    VkSamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext = nullptr;
    samplerCreateInfo.flags = 0;
    std::tie(
        samplerCreateInfo.minFilter,
        samplerCreateInfo.magFilter,
        samplerCreateInfo.mipmapMode) = ToTextureFilter(description.Filter);
    samplerCreateInfo.addressModeU = ToSamplerAddressMode(description.AddressU);
    samplerCreateInfo.addressModeV = ToSamplerAddressMode(description.AddressV);
    samplerCreateInfo.addressModeW = ToSamplerAddressMode(description.AddressW);
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = description.MinMipLevel;
    samplerCreateInfo.maxLod = description.MaxMipLevel;
    samplerCreateInfo.maxAnisotropy = description.MaxAnisotropy;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    auto result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkSampler");
    }

    ///@todo Not implemented

    // FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
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

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
