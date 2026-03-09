module github.com/mogemimi/pomdog/tools/cmd/spirv-shader-reflect

go 1.24.0

toolchain go1.24.2

replace github.com/mogemimi/pomdog/build/schemas => ../../../build/schemas-go/pomdogschemas

replace github.com/mogemimi/pomdog/tools/pkg/spirvreflect => ../../pkg/spirvreflect

replace github.com/mogemimi/pomdog/tools/pkg/stringhash => ../../pkg/stringhash

require (
	github.com/google/flatbuffers v25.2.10+incompatible
	github.com/mogemimi/pomdog/build/schemas v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/spirvreflect v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/stringhash v0.0.0-00010101000000-000000000000
)

require github.com/OneOfOne/xxhash v1.2.8 // indirect
