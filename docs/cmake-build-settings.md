# CMake Build Settings

Pomdog shares C++ build settings through INTERFACE targets and defines each
library in its own CMake file. For application integration and library selection, see
[Using Pomdog with CMake](using-pomdog-with-cmake.md). For platform build commands
and tool installation, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Build definition layout

| Location | Responsibility |
|---|---|
| Root `CMakeLists.txt` | Engine, application settings, and optional examples/tests |
| `cmake/pomdog/CMakeLists.txt` | Platform defaults/validation, shared policies, external dependencies, and includes of library definitions |
| `cmake/pomdog/pomdog_<name>.cmake` | One library's static target, alias, explicit sources, dependencies, and library-specific settings |
| `cmake/pomdog/asan.cmake` | MSVC sanitizer runtime-copy helper for executables |
| `cmake/<library>/CMakeLists.txt` | Bundled third-party sources, includes, and library-specific settings |
| `cmake/pomdog_app_settings/CMakeLists.txt` | Optional policies for applications and game libraries |
| `examples/*/CMakeLists.txt`, `tests/CMakeLists.txt` | Executables, platform entry points, and packaging |

Library definition files are read with plain `include()` in the engine directory. They are
not standalone CMake projects and introduce no extra directory scopes or
registration functions. Common target properties are applied through the
explicit `POMDOG_COMPILE_TARGETS` list. Source lists are explicit, without globbing.

Prefer `target_*` commands and generator expressions for sources, dependencies,
definitions, and flags. Use configure-time conditions for operations such as
platform validation, SDK discovery, and platform-specific dependency imports.
All engine library targets remain declared; `EXCLUDE_FROM_ALL` controls their
participation in the default build rather than their availability.

## Settings and propagation

| Target | Policy |
|---|---|
| `pomdog_build_settings` | Private engine warnings, optimization, internal include paths, CRT, and shared sanitizer/LTO settings |
| `pomdog::base` | Public include paths, C++23, header-affecting definitions, CRT, threading, and sanitizer/LTO usage requirements |
| `pomdog_lto_settings` | Release link-time optimization flags |
| `pomdog_sanitizer_settings` | AddressSanitizer compile/link flags and debug information |
| `pomdog_app_settings` | Optional application warnings, optimization, CRT, and shared sanitizer/LTO policies |

Public Debug and Windows definitions must agree between engine code and inline
code in consumer translation units. Engine warning options remain PRIVATE.
MSVC uses `/MTd` in Debug and `/MT` in Release. Keep that CRT choice explicit in
engine, dependency, and application settings even where base already propagates it.

Sources use headers rather than C++ named modules. Engine targets set
`CXX_SCAN_FOR_MODULES OFF` without changing the parent project's scanning policy.
The engine project enables C++; bundled C dependencies enable C in their own
project definitions. Pomdog supports Debug and Release configurations.

### Link-time optimization

`POMDOG_USE_LTO` normally defaults to ON. Generator expressions activate explicit
flags for Release only:

| Compiler | Compile flags | Link flags |
|---|---|---|
| MSVC | `/GL` | `/LTCG`, `/INCREMENTAL:NO` |
| Clang / AppleClang / GCC / Emscripten | `-flto` | `-flto` |

Set `-DPOMDOG_USE_LTO=OFF` to disable LTO. The shared INTERFACE settings propagate
to consumers, so executable and game-library targets need no per-target
optimization property. The flags do not enable PGO profile collection or use.
The chosen compiler, archiver, and linker must support this LTO flow; CMake does
not probe support or choose replacement flags.

Compiler references: [MSVC LTCG](https://learn.microsoft.com/en-us/cpp/build/reference/ltcg-link-time-code-generation?view=msvc-170),
[GCC optimization options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html),
and [Emscripten LTO](https://emscripten.org/docs/compiling/WebAssembly.html).

### AddressSanitizer

Enable `POMDOG_USE_ADDRESS_SANITIZER` to instrument the engine, its consumers,
and bundled libraries. A fresh ASan configuration defaults LTO to OFF; when
reusing a cache, set `-DPOMDOG_USE_LTO=OFF` explicitly. Requesting both is an error.
MemorySanitizer is not a supported configuration.

MSVC consumers must remove incompatible `/RTC` options and copy the ASan runtime
beside the executable using `pomdog_copy_asan_runtime`. Follow the complete
[sanitizer build instructions](developing-pomdog-game-engine.md#address-sanitizer-builds).

## External dependencies and generated code

The engine imports fmt, zlib, libpng, stb, and the selected platform dependencies
once. `png` links `zlib` PUBLIC; other engine or third-party targets can link the
same `zlib` target. A library declares dependencies where they are used, including
framework and WebGL/audio/VFS link requirements. Static-library PRIVATE
dependencies still contribute to the executable's final link.

The engine attaches `pomdog_sanitizer_settings` and `pomdog_lto_settings` PRIVATE
to each bundled library. Do not duplicate these flags inside its own CMakeLists.txt.
Library-specific warnings, definitions, optimization, CRT selection, and removal
of incompatible compiler defaults remain there. These wrappers receive shared
instrumentation through the engine; configuring a wrapper independently does not
apply the engine's policies automatically.

FlatBuffers generation is an external build step. The C++ include root is fixed
at the Pomdog checkout's `build/schemas-cpp`; Go output is in `build/schemas-go`.
The bootstrap recipe owns both outputs. Run bootstrap or build_tools after schema
changes; CMake neither invokes `flatc` nor checks freshness. Only compiled schema
consumers need those headers. See [the bootstrap procedure](developing-pomdog-game-engine.md#bootstrap-toolchain).

## Add a library

1. Place its public headers and implementation under `pomdog/<name>`. Reuse an
   existing library for small internal helpers that do not need independent use.
2. Add `cmake/pomdog/pomdog_<name>.cmake` with a static target, `pomdog::<name>`
   alias, explicit source list, and dependencies. Use `pomdog_graphics.cmake` as
   an example. Keep platform source selection and library-specific flags together.
3. Add one `include(pomdog_<name>.cmake)` and one entry in `POMDOG_COMPILE_TARGETS`
   in the engine CMakeLists.txt. Add external dependency imports there if needed.
4. Use PUBLIC dependencies for public-header requirements and PRIVATE for
   implementation dependencies. Keep dependencies directed toward lower layers;
   pass data or callbacks rather than making a lower layer depend on its caller.
5. Link the library from each application that uses it. Link development-only
   libraries from tools, keeping them out of product and shared game-logic targets.
6. Update the [library guide](using-pomdog-with-cmake.md#choose-libraries) and verify
   independent linking in Debug and without LTO. Check that unused libraries stay
   unbuilt when `POMDOG_EXCLUDE_FROM_ALL=ON`. See [engine library link tests](engine-library-link-tests.md).

Game-specific logic and editor code can follow the same dependency rules in the
game repository without becoming engine libraries.
