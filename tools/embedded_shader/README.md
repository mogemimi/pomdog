# embedded_shader

## Build

```sh
cd path/to/pomdog/tools/embedded_shader

# Install the following golang packages
go get -u github.com/pkg/errors

# Build
go build
```

## Run

```sh
cd path/to/pomdog

# Convert GLSL to embedded C++ code
./tools/embedded_shader/embedded_shader \
    src/Experimental/Graphics/Shaders/GLSL/LineBatch_VS.glsl

# Convert a vertex shader from HLSL to embedded C++ code
./tools/embedded_shader/embedded_shader \
    -fxc "C:\Program Files (x86)\Windows Kits\10\bin\10.0.17134.0\x64\fxc.exe" \
    -entrypoint LineBatchVS \
    -profile vs_4_0 \
    src/Experimental/Graphics/Shaders/HLSL/LineBatch_VS.hlsl

# Convert a pixel shader from HLSL to embedded C++ code
./tools/embedded_shader/embedded_shader \
    -fxc "C:\Program Files (x86)\Windows Kits\10\bin\10.0.17134.0\x64\fxc.exe" \
    -entrypoint LineBatchPS \
    -profile ps_4_0 \
    src/Experimental/Graphics/Shaders/HLSL/LineBatch_PS.hlsl

# Convert Metal to embedded C++ code
./tools/embedded_shader/embedded_shader \
    src/Experimental/Graphics/Shaders/Metal/LineBatch.metal
```
