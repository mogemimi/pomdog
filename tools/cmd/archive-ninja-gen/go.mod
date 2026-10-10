module github.com/mogemimi/pomdog/tools/cmd/archive-ninja-gen

go 1.26.0

replace github.com/mogemimi/pomdog/tools/pkg/archives => ../../pkg/archives

replace github.com/mogemimi/pomdog/tools/pkg/ninja => ../../pkg/ninja

require (
	github.com/mogemimi/pomdog/tools/pkg/archives v0.0.0-00010101000000-000000000000
	github.com/mogemimi/pomdog/tools/pkg/ninja v0.0.0-00010101000000-000000000000
)

require github.com/pelletier/go-toml/v2 v2.4.3 // indirect
