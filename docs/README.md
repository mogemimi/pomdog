# Pomdog Documentation

## For game developers

- [Getting Started](getting-started.md): Create a new project and build it
- [Using Pomdog with CMake](using-pomdog-with-cmake.md): Select libraries and share game logic between applications
- [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md): Offline processing policy, data roles, asset builds, and runtime loading
- [Shipping](shipping.md): Create distributable packages for Windows, Linux, and more

## For engine developers

- [Developing Pomdog Game Engine](developing-pomdog-game-engine.md): Prerequisites, build instructions, code formatting, sanitizer builds
- [Coding Guidelines](coding-guidelines.md): C++ conventions, error handling, API design, comments, and tests
- [CMake Build Settings](cmake-build-settings.md): Compiler policies, dependency ownership, and adding libraries
- [Running the Tests](running-the-tests.md): Build and run unit tests on each platform
- [Engine Library Link Tests](engine-library-link-tests.md): Verify independent CMake consumers and their dependencies
- [Building for Linux](building-linux.md): Custom toolchains, static linking, and shared library bundling
- [Building with Emscripten](building-emscripten.md): Build for WebAssembly
- [Building with Vulkan Backend](building-vulkan.md): Enable and build with the Vulkan graphics backend
- [Setting Up Development Environment on Ubuntu](setting-up-development-environment-on-ubuntu.md): Install toolchains and libraries on Ubuntu
- [Building in Docker Container](building-in-docker-container.md): Use Docker images for CI and development

## Reference

- [Architecture](architecture.md): Engine overview, features, platform backends, and library structure
- [Audio](audio.md): Audio subsystem, loading, streaming, playback, and the offline asset pipeline
- [Headless Mode](headless-mode.md): Current platform behavior and limits of GPU headless execution
- [Shader Compilation](shader-compilation.md): Slang pipeline, SPIR-V post-processing, and cross-compilation details
- [Open Source Software Used in Pomdog](open-source-software-used-in-pomdog.md): Third-party dependencies and licenses
- [Versioning](versioning.md): How Pomdog versioning works
- [Deploying API Documentation](deploying-api-documentation.md): Generate Doxygen documentation
