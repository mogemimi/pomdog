# generate_gamepad_db

## Build

```sh
cd path/to/pomdog/tools/generate_gamepad_db

# Build
go build
```

## Run

```sh
cd path/to/pomdog

# Generate a header file from game controller DB
go run tools/generate_gamepad_db/main.go \
    -i dependencies/SDL_GameControllerDB/gamecontrollerdb.txt \
    -o dependencies/vendor/SDL_GameControllerDB/SDL_GameControllerDB.h
```
