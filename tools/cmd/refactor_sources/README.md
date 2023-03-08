# refactor_sources

## Build

```sh
cd path/to/pomdog/tools/refactor_sources

# Refactor all source files in source directories
go run tools/refactor_sources/main.go -i include
go run tools/refactor_sources/main.go -i src

# Build
go build
```

## Run

```sh
cd path/to/pomdog

# Refactor source file
./tools/cmd/refactor_sources/refactor_sources src/Application/Timer.cpp

# Refactor all source files
./tools/cmd/refactor_sources/refactor_sources --all
```
