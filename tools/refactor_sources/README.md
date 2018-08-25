# refactor_sources

## Build

```sh
cd path/to/pomdog/tools/refactor_sources

# Install the following golang packages
go get -u github.com/pkg/errors

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
./tools/refactor_sources/refactor_sources src/Application/Timer.cpp
```
