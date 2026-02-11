# Installing CMake on Linux

CMake depends on the OpenSSL library. On Ubuntu or ArchLinux, you can install it with the following command:

```sh
# On Ubuntu
sudo apt-get install make build-essential libssl-dev

# On ArchLinux
sudo pacman -S openssl
```

To build CMake locally, run the following command: 

```sh
cd path/to/your/directory

mkdir cmakebuild
cd cmakebuild

wget https://github.com/Kitware/CMake/releases/download/v3.21.4/cmake-3.21.4.tar.gz
tar -xf cmake-3.21.4.tar.gz
cd cmake-3.21.4
./bootstrap --parallel=4 --prefix=/usr/local
make -j4
sudo make install

cd ../..
rm -R cmakebuild
```
