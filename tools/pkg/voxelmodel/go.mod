module github.com/mogemimi/pomdog/tools/pkg/voxelmodel

go 1.26.0

replace github.com/mogemimi/pomdog/build/schemas => ../../../build/schemas-go/pomdogschemas

replace github.com/mogemimi/pomdog/tools/pkg/magicavoxel => ../magicavoxel

require (
	github.com/google/flatbuffers v25.2.10+incompatible
	github.com/mogemimi/pomdog/build/schemas v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/magicavoxel v0.0.0-00010101000000-000000000000
)
