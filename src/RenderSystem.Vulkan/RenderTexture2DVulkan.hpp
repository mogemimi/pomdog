// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Texture2DVulkan.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class RenderTarget2DVulkan final : public NativeRenderTarget2D {
public:
    RenderTarget2DVulkan(
        VkDevice device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

private:
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
