ROOT_DIR="$PWD"

cd $ROOT_DIR/tools/cmd/bootstrap-toolchain
go build -o $ROOT_DIR/build/tools/bootstrap-toolchain
cd $ROOT_DIR
./build/tools/bootstrap-toolchain \
    --pomdogdir $ROOT_DIR \
    --builddir $ROOT_DIR/build \
    --appdir $ROOT_DIR \
    --skip-cpp \
    --skip-download \
    $ROOT_DIR/tools/cmd/bootstrap-toolchain/bootstrap.toml
