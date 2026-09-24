# Using Pomdog with CMake

Pomdog provides a set of static libraries. Link the libraries
your application uses; CMake builds and links their dependencies automatically.
The `pomdog::` names are CMake aliases for these libraries.

For a complete game template, start with [Getting Started](getting-started.md).
To add Pomdog to an existing project, follow the examples below. See [CMake Build Settings](cmake-build-settings.md)
for compiler policies and maintaining the engine's build definitions.

## Prepare the engine

Use CMake 3.31 or later and a C++23 compiler supported by the
[development guide](developing-pomdog-game-engine.md#prerequisites).
Initialize Pomdog's submodules and run `tools/script/bootstrap.sh` from the
Pomdog checkout before configuring the project. This prepares tools and generates
FlatBuffers code in `build/schemas-cpp` and `build/schemas-go` in that checkout.
After schema changes, run `tools/script/build_tools.sh` again. CMake consumes
generated headers; it does not run `flatc` or update stale output.

Use a separate CMake build directory for each platform/toolchain. Windows and
Xcode builds select Debug or Release with `--config`; single-configuration Ninja
builds select it with `-DCMAKE_BUILD_TYPE=Debug` or `Release` during configure.

## Add a CPU-only executable

For a workspace containing a `pomdog/` checkout and `simulation/main.cpp`:

```cmake
cmake_minimum_required(VERSION 3.31)
project(my_game_workspace CXX)

set(POMDOG_EXCLUDE_FROM_ALL ON CACHE BOOL "Build only required engine libraries")
add_subdirectory(pomdog/cmake/pomdog pomdog_build)

add_executable(simulation simulation/main.cpp)
target_link_libraries(simulation PRIVATE pomdog::math pomdog::random)
# Keep the application's CRT choice explicit, matching Pomdog.
target_compile_options(simulation PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:$<IF:$<CONFIG:Debug>,/MTd,/MT>>
)
```

Build this executable with:

```sh
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --config Debug --target simulation
```

Include headers using paths such as `pomdog/math/vector2.h`. Library targets
provide the include directory and C++23 requirement. This example builds no GPU,
audio, network, or window implementation. The executable owns its loop and
chooses the simulation step, input, output, and exit conditions for its game.
A GPU-free GameHost implementation is not provided.

## Choose libraries

Link libraries whose APIs your code directly uses. Internal dependencies are
resolved transitively, including PRIVATE dependencies of static libraries that
are needed when linking an executable.

| Target | Purpose |
|---|---|
| `pomdog::base` | Basic types, chrono, logging, memory, signals, and general utilities |
| `pomdog::math` | Vectors, matrices, geometry, and easing |
| `pomdog::random` | Random-number generators |
| `pomdog::filesystem` | Platform file and directory access |
| `pomdog::vfs` | Virtual filesystem, archive access, and optional default mounting helper |
| `pomdog::input` | Device APIs, input state, and gamepad implementations |
| `pomdog::gpu` | GPU resources, commands, and graphics backends |
| `pomdog::audio` | Audio devices, clips, and playback |
| `pomdog::network` | Network services, streams, and HTTP utilities |
| `pomdog::application` | Shared Game, GameHost, GameSetup definitions and frame-rate limiter |
| `pomdog::application_gpu` | Platform hosts, windows, and event adapters |
| `pomdog::content_utility` | Binary-reading helpers for asset loaders |
| `pomdog::content_gpu` | Shader and texture loading |
| `pomdog::content_audio` | Audio loading and streaming decoders |
| `pomdog::content_input` | Game-controller database loading |
| `pomdog::graphics` | Sprite, text, primitive, and effect rendering |
| `pomdog::ecs` | Entity component system |
| `pomdog::console` | Console output |
| `pomdog::experimental_async` | Async schedulers and tasks |
| `pomdog::experimental_gltf` | glTF data and loading |
| `pomdog::experimental_gui` | Editor GUI widgets |
| `pomdog::experimental_image` | CPU images and SVG loading |
| `pomdog::experimental_image_effects` | Post-processing effects |
| `pomdog::experimental_magicavoxel` | MagicaVoxel import and export |
| `pomdog::experimental_particles` | Particle and beam systems |
| `pomdog::experimental_skeletal2d` | 2D skeletal animation |
| `pomdog::experimental_spine` | Spine data loading |
| `pomdog::experimental_texture_atlas` | Static and dynamic texture atlases |

Each library produces a static archive. Link its `pomdog::` target so CMake also
applies its include paths, compiler requirements, and library dependencies.

### Runtime and platform constraints

- `application_gpu` uses GPU, input, audio, and desktop network implementations.
  `GameHostOptions` can skip some initialization at runtime, but does not change
  those build dependencies. See [Headless Mode](headless-mode.md) for hidden-window
  and offscreen-rendering limits.
- Browser `network` can be built and linked, but its TCP / UDP / TLS transports
  are stubs. Browser HTTP / WebSocket transport implementations are not provided.
  Desktop socket code and mbedTLS are excluded on Emscripten.
- Experimental features are separate libraries. Link only those used by the
  application; `POMDOG_EXCLUDE_FROM_ALL=ON` keeps unused ones out of ALL.
- Content loaders use VFS. The application prepares and passes the VFS context;
  GameHost does not mount assets. See [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md).

## Share game logic between applications

Import the engine once in the workspace's root CMakeLists.txt, then add each
library and executable directory. The following example assumes `game_logic`,
`simulation`, and `viewer` each contain their own CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.31)
project(my_game_workspace CXX)

set(POMDOG_EXCLUDE_FROM_ALL ON CACHE BOOL "Build only required engine libraries")
add_subdirectory(pomdog/cmake/pomdog pomdog_build)
add_subdirectory(pomdog/cmake/pomdog_app_settings app_settings_build)
add_subdirectory(game_logic)
add_subdirectory(simulation)
add_subdirectory(viewer)
```

```cmake
# game_logic/CMakeLists.txt
add_library(game_logic STATIC simulation.cpp)
target_link_libraries(game_logic PUBLIC pomdog::math PRIVATE pomdog::random)

# simulation/CMakeLists.txt
add_executable(simulation main.cpp)
target_link_libraries(simulation PRIVATE game_logic pomdog_app_settings)

# viewer/CMakeLists.txt
add_executable(viewer main.cpp)
target_link_libraries(viewer PRIVATE
    game_logic pomdog_app_settings pomdog::application_gpu
    pomdog::graphics pomdog::content_gpu pomdog::vfs
)
```

These snippets show target relationships. A graphical executable also needs the
platform entry point and packaging settings shown in
[examples/quickstart](../examples/quickstart/CMakeLists.txt).
Keep GPU-dependent game code in a separate library from shared simulation code
so logic tests and CLI tools do not inherit rendering dependencies.

`pomdog_app_settings` is an optional shared application policy: warnings,
optimization, explicit MSVC CRT selection, sanitizer/LTO settings, and Browser
executable settings. Game libraries can also link it PRIVATE when adopting that
policy. Otherwise keep their CRT selection explicit as in the CPU example.
Engine warning flags themselves are not imposed on consumers.

Generated quickstart projects import Pomdog when configured as top-level
projects. When added under a parent, that parent must import both the engine and
application settings first. Point each generated project's `POMDOG_DIR` at the
same checkout; do not add another copy of Pomdog from each executable directory.

Development-only libraries follow the same rules: link them from editors or
viewers, and keep them out of the shared game-logic dependency graph. Their use
is independent of Debug/Release; a Release editor may still need them.

## Select what ALL builds

Building a named executable always builds that executable and its dependencies.
The default ALL build also builds every declared engine library. Set
`-DPOMDOG_EXCLUDE_FROM_ALL=ON` to exclude unused engine libraries from ALL.
It leaves targets available for explicit builds and linked consumers.

When adding the repository root instead of `cmake/pomdog`, the root also exposes:

| Option | Effect |
|---|---|
| `POMDOG_BUILD_EXAMPLES` | Include the example applications |
| `POMDOG_BUILD_TESTS` | Include the engine test executable |
| `POMDOG_BUILD_QUICKSTART` | Include quickstart when examples are enabled |
| `POMDOG_BUILD_PONG` | Include pong when examples are enabled |
| `POMDOG_BUILD_FEATURE_SHOWCASE` | Include feature_showcase when examples are enabled |

Examples/tests default to ON for a top-level engine checkout and OFF when it is
embedded in another project. Per-example defaults follow the initial examples
option; cached choices persist on later configurations. Pong and feature_showcase
link the experimental libraries they use. Tests are selected as a complete executable.

For example, to configure library targets and build only math:

```sh
cmake -S pomdog -B build/pomdog-libraries \
    -DPOMDOG_BUILD_EXAMPLES=OFF \
    -DPOMDOG_BUILD_TESTS=OFF \
    -DPOMDOG_EXCLUDE_FROM_ALL=ON \
    -DPOMDOG_USE_LTO=OFF
cmake --build build/pomdog-libraries --config Debug --target pomdog_math
```

Excluding compilation does not skip configure-time platform validation or SDK
lookup. For example, explicitly enabling Vulkan still requires its SDK. Bundled
third-party libraries are defined once and built when required by selected
engine libraries. The examples/tests switches are separate from library exclusion.
