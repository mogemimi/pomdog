# Building with Vulkan Backend

Pomdog supports a Vulkan graphics backend on Windows.
This guide explains how to set up the Vulkan SDK, build Pomdog with Vulkan enabled, and select Vulkan at runtime.

> **Platform support:** The Vulkan backend is currently available on **Windows** only.
> Linux and macOS (via MoltenVK) support is planned for a future release.

## Prerequisites

In addition to the [standard prerequisites](developing-pomdog-game-engine.md#prerequisites), you need:

- [Vulkan SDK](https://vulkan.lunarg.com/) 1.3 or later (1.4.x recommended)
  - The installer sets the `VULKAN_SDK` environment variable automatically.
  - CMake uses `find_package(Vulkan)` to locate the SDK.

Verify your installation:

```sh
# Check the environment variable
echo %VULKAN_SDK%
# e.g. C:\VulkanSDK\1.4.341.1

# Check that vulkaninfo runs
vulkaninfo --summary
```

## CMake options

The Vulkan backend is controlled by the `POMDOG_USE_VULKAN` CMake option.
It is `OFF` by default. Backend options can be combined freely:

| Option | Default | Description |
|:---|:---|:---|
| `POMDOG_USE_VULKAN` | `OFF` | Enable the Vulkan graphics backend |
| `POMDOG_USE_DIRECT3D11` | Platform default | Enable the Direct3D 11 graphics backend |
| `POMDOG_USE_GL4` | Platform default | Enable the OpenGL 4 graphics backend |

When `POMDOG_USE_VULKAN=ON` is set, CMake will:

1. Find the Vulkan SDK via `find_package(Vulkan REQUIRED)`.
2. Add `POMDOG_USE_VULKAN` as a compile definition.
3. Compile and link the `pomdog/gpu/vulkan/` source files.

## Building on Windows

```sh
cd path/to/pomdog

# Configure with Vulkan enabled (and Direct3D 11 as fallback)
cmake -Bbuild/windows_vulkan -H. \
    -DPOMDOG_USE_VULKAN=ON \
    -DPOMDOG_USE_DIRECT3D11=ON \
    -DPOMDOG_USE_GL4=OFF

# Build
cmake --build build/windows_vulkan --config Debug

# Run the feature showcase example
./build/windows_vulkan/examples/feature_showcase/Debug/feature_showcase.exe
```

To use a specific Visual Studio generator:

```sh
# Visual Studio 2026
cmake -Bbuild/windows_vulkan -H. -G "Visual Studio 18" \
    -DPOMDOG_USE_VULKAN=ON \
    -DPOMDOG_USE_DIRECT3D11=ON \
    -DPOMDOG_USE_GL4=OFF
```

## Selecting the Vulkan backend at runtime

Use `setGraphicsBackend()` on the Bootstrap to select the graphics backend.
This method is available on all platforms (Windows, macOS, and Linux).

### Windows

```cpp
#include "pomdog/platform/win32/bootstrap_win32.h"
#include "pomdog/gpu/graphics_backend.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    using namespace pomdog;

    win32::Bootstrap bootstrap;
    bootstrap.setInstance(hInstance);
    bootstrap.setCommandShow(nCmdShow);
    bootstrap.setBackBufferSize(800, 480);

    // Select Vulkan backend
    bootstrap.setGraphicsBackend(gpu::GraphicsBackend::Vulkan);

    bootstrap.run([]() {
        return std::make_unique<YourGame>();
    });

    return 0;
}
```

## Validation layers

In debug builds (`POMDOG_DEBUG_BUILD` / `_DEBUG`), the Vulkan validation layer `VK_LAYER_KHRONOS_validation` is automatically enabled.
Validation errors are reported to the debug output.
Make sure the Vulkan SDK's layer binaries are accessible (they are included in the standard SDK installation).

To see validation messages in Visual Studio, run the program under the debugger — messages appear in the Output window.

## Known limitations

- **MSAA**: Multi-sample anti-aliasing is not yet supported in the Vulkan backend. The sample count is fixed at 1.
- **Platform**: Only Windows is supported. Linux (with Xlib surface) and macOS (with MoltenVK) support is planned.
