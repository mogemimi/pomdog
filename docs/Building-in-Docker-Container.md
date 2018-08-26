# Building in Docker Container

Pomdog uses the following docker containers for building and running test in CI.

- GitHub: [mogemimi/gamedev-docker](https://github.com/mogemimi/gamedev-docker)
- Docker Hub:
  - [mogemimi/gamedev-ubuntu](https://hub.docker.com/r/mogemimi/gamedev-ubuntu) (Docker image based on Ubuntu)
  - [mogemimi/gamedev-archlinux](https://hub.docker.com/r/mogemimi/gamedev-archlinux) (Docker image based on Arch Linux)

To build your Linux application with Pomdog inside Docker container, run the following steps.

1. Pulling the docker image

    ```sh
    docker pull mogemimi/gamedev-ubuntu
    ```

2. Running the docker image

    ```sh
    docker run -t -i mogemimi/gamedev-ubuntu
    ```

3. Cloning the repository

    ```sh
    git clone https://github.com/mogemimi/pomdog.git && cd pomdog
    git submodule update --init
    ```

4. Building and running test

    Please see [Running the Tests](Running-the-Tests.md).
