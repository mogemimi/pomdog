# git_archive_all

```sh
cd path/to/pomdog

# NOTE: Create an archive of files with submodules, from a HEAD tree.
go run tools/cmd/git_archive_all/main.go

# NOTE: options
go run tools/cmd/git_archive_all/main.go \
    -p pomdog \
    -C path/to/pomdog \
    -o path/to/output-directory/pomdog.zip
```
