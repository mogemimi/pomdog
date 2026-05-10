// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/texture_address_mode.h"
#include "pomdog/gpu/texture_filter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// SamplerDesc describes the sampling state for a texture sampler.
struct POMDOG_EXPORT SamplerDesc final {
    /// Maximum anisotropy level used when filter is Anisotropic; clamped to the hardware limit.
    u32 maxAnisotropy = 0;

    /// Minimum mip level that can be selected during sampling.
    f32 minMipLevel = 0;

    /// Maximum mip level that can be selected during sampling.
    f32 maxMipLevel = std::numeric_limits<f32>::max();

    /// Offset added to the computed mip level before sampling.
    f32 mipmapLevelOfDetailBias = 0;

    /// Filtering method for minification, magnification, and mip-level selection.
    TextureFilter filter = TextureFilter::Linear;

    /// Texture addressing mode along the horizontal (U) axis.
    TextureAddressMode addressU = TextureAddressMode::Clamp;

    /// Texture addressing mode along the vertical (V) axis.
    TextureAddressMode addressV = TextureAddressMode::Clamp;

    /// Texture addressing mode along the depth (W) axis; used for 3D textures.
    TextureAddressMode addressW = TextureAddressMode::Clamp;

    /// Comparison function for depth-comparison samplers; ignored for regular samplers.
    ComparisonFunction comparisonFunction = ComparisonFunction::Never;

    //std::array<f32, 4> borderColor;

    /// Returns a sampler descriptor equivalent to `createLinearClamp()`.
    [[nodiscard]] static SamplerDesc
    createDefault() noexcept;

    /// Returns a sampler descriptor using anisotropic filtering with clamped texture addressing.
    [[nodiscard]] static SamplerDesc
    createAnisotropicClamp() noexcept;

    /// Returns a sampler descriptor using anisotropic filtering with wrapped texture addressing.
    [[nodiscard]] static SamplerDesc
    createAnisotropicWrap() noexcept;

    /// Returns a sampler descriptor using linear filtering with clamped texture addressing.
    [[nodiscard]] static SamplerDesc
    createLinearClamp() noexcept;

    /// Returns a sampler descriptor using linear filtering with wrapped texture addressing.
    [[nodiscard]] static SamplerDesc
    createLinearWrap() noexcept;

    /// Returns a sampler descriptor using point (nearest-neighbor) filtering with clamped texture addressing.
    [[nodiscard]] static SamplerDesc
    createPointClamp() noexcept;

    /// Returns a sampler descriptor using point (nearest-neighbor) filtering with wrapped texture addressing.
    [[nodiscard]] static SamplerDesc
    createPointWrap() noexcept;
};

} // namespace pomdog::gpu
