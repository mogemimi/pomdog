// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// GraphicsDeviceInfo describes the GPU behind a GraphicsDevice.
struct GraphicsDeviceInfo final {
    /// The name of the GPU or renderer, as reported by the driver.
    std::string deviceName;

    /// The version of the graphics API that the device supports. The format
    /// depends on the backend: the feature level such as "11_1" on Direct3D 11,
    /// the GL_VERSION string on OpenGL and WebGL, the apiVersion of the
    /// physical device such as "1.3.280" on Vulkan, and "Metal 3" or "Metal 2"
    /// on Metal.
    std::string apiVersion;
};

} // namespace pomdog::gpu
