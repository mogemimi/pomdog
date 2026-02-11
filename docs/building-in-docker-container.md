# Building in Docker Container

Pomdog provides Docker images for CI builds and development:

- [mogemimi/gamedev-ubuntu](https://hub.docker.com/r/mogemimi/gamedev-ubuntu) — Ubuntu-based
- [mogemimi/gamedev-archlinux](https://hub.docker.com/r/mogemimi/gamedev-archlinux) — Arch Linux-based
- [mogemimi/gamedev-emscripten](https://hub.docker.com/r/mogemimi/gamedev-emscripten/) — Emscripten
- Source: [mogemimi/gamedev-docker](https://github.com/mogemimi/gamedev-docker)

## Quick start

```sh
# Pull the image
docker pull mogemimi/gamedev-ubuntu

# Start a container
docker run -t -i mogemimi/gamedev-ubuntu

# Inside the container: clone and build
git clone https://github.com/mogemimi/pomdog.git && cd pomdog
git submodule update --init --recursive
```

For build and test commands, see [Running the Tests](running-the-tests.md).
