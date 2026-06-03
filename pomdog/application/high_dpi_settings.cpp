// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/high_dpi_settings.h"

namespace pomdog {

[[nodiscard]] f32
computeEffectivePixelRatio(const HighDPISettings& settings, f32 rawPixelRatio) noexcept
{
    if (settings.mode == HighDPIMode::Disabled) {
        return 1.0f;
    }
    const f32 ratio = (rawPixelRatio > 0.0f) ? rawPixelRatio : 1.0f;
    return (ratio < settings.maxPixelRatio) ? ratio : settings.maxPixelRatio;
}

[[nodiscard]] f32
computeUnclampedPixelRatio(const HighDPISettings& settings, f32 rawPixelRatio) noexcept
{
    if (settings.mode == HighDPIMode::Disabled) {
        return 1.0f;
    }
    return (rawPixelRatio > 0.0f) ? rawPixelRatio : 1.0f;
}

} // namespace pomdog
