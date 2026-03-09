module github.com/mogemimi/pomdog/tools/cmd/shader-archive-gen

go 1.24.0

replace github.com/mogemimi/pomdog/tools/pkg/archives => ../../pkg/archives

require (
	github.com/mogemimi/pomdog/tools/pkg/archives v0.0.0-00010101000000-000000000000
	github.com/pelletier/go-toml/v2 v2.2.4
)
