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
./build/windows_vulkan/examples/feature_showcase/Debug/feature_showcase.exe \
    --archive-file build/feature_showcase/shipping/windows/content.idx \
    --graphics-backend vulkan
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

Pass `--graphics-backend vulkan` on the command line.
The Bootstrap forwards the command-line arguments to `GameSetup::configure()`, which
calls `parseGraphicsBackend()` and stores the result in `GameHostOptions::graphicsBackend`.

```sh
# Run with the Vulkan backend
./build/windows_vulkan/examples/feature_showcase/Debug/feature_showcase.exe \
    --archive-file build/feature_showcase/shipping/windows/content.idx \
    --graphics-backend vulkan
```

The wiring is done in two places:

**`platform/win32/main.cpp`** — forwards the command-line arguments to `GameSetup`:

```cpp
#include "game_setup.h"
#include "pomdog/application/win32/bootstrap_win32.h"
#include "pomdog/pomdog.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    using namespace pomdog;

    win32::Bootstrap bootstrap;
    bootstrap.setInstance(hInstance);
    bootstrap.setCommandShow(nCmdShow);
    bootstrap.setCommandLineArgs(__argc, const_cast<const char* const*>(__argv));

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        OutputDebugString(err->toString().c_str());
    });

    bootstrap.run(createGameSetup());
    return 0;
}
```

**`source/game_setup.cpp`** — parses `--graphics-backend` and sets the backend:

```cpp
#include "game_setup.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/graphics_backend_helper.h"
#include "pomdog/utility/cli_parser.h"

class GameSetupImpl final : public pomdog::GameSetup {
public:
    [[nodiscard]] std::unique_ptr<pomdog::Error>
    configure(pomdog::GameHostOptions& options,
              std::span<const char* const> args) override
    {
        std::string graphicsBackend;
        pomdog::CLIParser cli;
        cli.add(&graphicsBackend, "graphics-backend",
                "graphics backend (d3d11, gl4, vulkan, metal, webgl)");
        if (auto err = cli.parse(args); err != nullptr) {
            return pomdog::errors::wrap(std::move(err), "failed to parse args");
        }

        if (!graphicsBackend.empty()) {
            if (auto b = pomdog::parseGraphicsBackend(graphicsBackend); b) {
                options.graphicsBackend = *b;
            }
        }

        options.backBufferWidth  = 800;
        options.backBufferHeight = 480;
        return nullptr;
    }
    // ...
};
```

Accepted `--graphics-backend` values: `d3d11`, `gl4`, `vulkan`, `metal`, `webgl`.
The match is **case-sensitive**.

## Validation layers

In debug builds (`POMDOG_DEBUG_BUILD` / `_DEBUG`), the Vulkan validation layer `VK_LAYER_KHRONOS_validation` is automatically enabled.
Validation errors are reported to the debug output.
Make sure the Vulkan SDK's layer binaries are accessible (they are included in the standard SDK installation).

To see validation messages in Visual Studio, run the program under the debugger — messages appear in the Output window.

## Known limitations

- **MSAA**: Multi-sample anti-aliasing is not yet supported in the Vulkan backend. The sample count is fixed at 1.
- **Platform**: Only Windows is supported. Linux (with Xlib surface) and macOS (with MoltenVK) support is planned.
