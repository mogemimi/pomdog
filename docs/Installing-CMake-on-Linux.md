# Installing CMake on Linux

```sh
cd path/to/your/directory

mkdir cmakebuild
cd cmakebuild

wget https://cmake.org/files/v3.12/cmake-3.12.0.tar.gz
tar -xf cmake-3.12.0.tar.gz
cd cmake-3.12.0
./bootstrap --parallel=4 --prefix=/usr/local
make -j4
sudo make install

cd ../..
rm -R cmakebuild
```
