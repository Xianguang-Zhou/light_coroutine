# light_coroutine

"light_coroutine" is a C coroutine library for Unix/Linux.

## Dependencies

To compile "light_coroutine" from source you need following packages:

* C compiler
* C library
* make
* cmake >= 2.6

For Ubuntu 16.04, install dependencies by this command:

```sh
sudo apt-get install gcc libc6-dev make cmake
```

## Getting source code

```sh
wget -c https://github.com/Xianguang-Zhou/light_coroutine/archive/master.zip
unzip master.zip
mv light_coroutine-master light_coroutine
```

or

```sh
git clone --depth=1 https://github.com/Xianguang-Zhou/light_coroutine.git
```

or

```sh
git clone https://github.com/Xianguang-Zhou/light_coroutine.git
```

## Building

### Creating a separate directory for building

```sh
mkdir light_coroutine_build
cd light_coroutine_build
```

### Generating Makefile by CMake

```sh
cmake -DCMAKE_BUILD_TYPE=Release ../light_coroutine
```

To develop LightCoroutine, generate Makefile and Eclipse project files by this command:

```sh
cmake -G "Eclipse CDT4 - Unix Makefiles" \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_ECLIPSE_VERSION=3.7 ../light_coroutine
```

### Compiling and Linking

```sh
make
```

## Installation

```sh
sudo make install
```

## Report bug

If you find any problem, please file this bug in
https://github.com/Xianguang-Zhou/light_coroutine/issues, thanks.
