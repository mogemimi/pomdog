# Versioning

Pomdog follows [Semantic Versioning](http://semver.org/).

## Bumping the version

1. Edit the version header:

    ```sh
    vim pomdog/basic/version.h
    git add pomdog/basic/version.h
    git commit -m "Bump version to 0.1.3"
    ```

2. Tag the release:

    ```sh
    git tag v0.1.3
    git push origin v0.1.3
    ```
