// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/graphics_backend_helper.h"
#include "pomdog/gpu/graphics_backend.h"

namespace pomdog {

[[nodiscard]] std::optional<gpu::GraphicsBackend>
parseGraphicsBackend(std::string_view name) noexcept
{
    if (name == "d3d11") {
        return gpu::GraphicsBackend::Direct3D11;
    }
    if (name == "gl4") {
        return gpu::GraphicsBackend::OpenGL4;
    }
    if (name == "vulkan") {
        return gpu::GraphicsBackend::Vulkan;
    }
    if (name == "metal") {
        return gpu::GraphicsBackend::Metal;
    }
    if (name == "webgl") {
        return gpu::GraphicsBackend::WebGL;
    }
    return std::nullopt;
}

} // namespace pomdog
