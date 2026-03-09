module github.com/mogemimi/pomdog/tools/cmd/archive-inspect

go 1.24.0

replace github.com/mogemimi/pomdog/build/schemas => ../../../build/schemas-go/pomdogschemas

require github.com/mogemimi/pomdog/build/schemas v0.0.0-00010101000000-000000000000

require github.com/google/flatbuffers v25.2.10+incompatible // indirect
