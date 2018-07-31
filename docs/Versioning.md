# Versioning

## How does Pomdog versioning work?

Pomdog follows [semver](http://semver.org/). To bump the version of the engine, follow the following steps:

1. Edit source code

    ```sh
    vim include/Pomdog/Basic/Version.hpp
    git add include/Pomdog/Basic/Version.hpp
    git commit -m "Bump version to 0.1.3"
    ```

2. Tagging

    ```sh
    git tag v0.1.3
    git push origin v0.1.3
    ```
