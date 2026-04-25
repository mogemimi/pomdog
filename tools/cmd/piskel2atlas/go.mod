module github.com/mogemimi/pomdog/tools/cmd/piskel2atlas

go 1.25.0

replace github.com/mogemimi/pomdog/build/schemas => ../../../build/schemas-go/pomdogschemas

replace github.com/mogemimi/pomdog/tools/pkg/depfile => ../../pkg/depfile

replace github.com/mogemimi/pomdog/tools/pkg/piskel => ../../pkg/piskel

replace github.com/mogemimi/pomdog/tools/pkg/stringhash => ../../pkg/stringhash

require (
	github.com/google/flatbuffers v25.2.10+incompatible
	github.com/mogemimi/pomdog/build/schemas v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/depfile v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/piskel v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/stringhash v0.0.0-00010101000000-000000000000
	github.com/pelletier/go-toml/v2 v2.3.0
)

require github.com/OneOfOne/xxhash v1.2.8 // indirect
