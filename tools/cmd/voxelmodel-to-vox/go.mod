module github.com/mogemimi/pomdog/tools/cmd/voxelmodel-to-vox

go 1.26.0

replace github.com/mogemimi/pomdog/build/schemas => ../../../build/schemas-go/pomdogschemas

replace github.com/mogemimi/pomdog/tools/pkg/magicavoxel => ../../pkg/magicavoxel

replace github.com/mogemimi/pomdog/tools/pkg/voxelmodel => ../../pkg/voxelmodel

require (
	github.com/mogemimi/pomdog/build/schemas v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/magicavoxel v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/voxelmodel v0.0.0-00010101000000-000000000000
)

require github.com/google/flatbuffers v25.2.10+incompatible // indirect
