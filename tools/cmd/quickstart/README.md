# quickstart

## Build

```sh
cd path/to/pomdog/tools/quickstart

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
cd path/to/your/develop/directory

# Make sure pomdog directory exists
ls pomdog

# Run
./pomdog/tools/quickstart/quickstart
```

e.g.

```sh
$ ./pomdog/tools/quickstart/quickstart
> Where is a Pomdog directory? (e.g. path/to/pomdog) pomdog
> Where do you want to create your new gamedev project? [.]
> What is your project name? (e.g. MyGame) MyGame
> What is your project URL? [com.example.MyGame]
Done.
```

You can also use non-interactive mode for CI with the `-non-interactive` option:

```sh
./pomdog/tools/quickstart/quickstart \
    -non-interactive \
    -pomdog path/to/pomdog \
    -o . \
    -name MyGame \
    -url com.example.MyGame
```
