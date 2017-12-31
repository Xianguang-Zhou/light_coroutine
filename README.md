# light_coroutine

"light_coroutine" is a C coroutine library for Linux, Unix and Windows.

## Dependencies

To compile "light_coroutine" from source you need following packages:

* C, C++ compiler
* C, C++ library
* make
* cmake >= 2.6

For Ubuntu 16.04, install dependencies by this command:

```sh
sudo apt-get install g++ make cmake
```

Install Mingw-w64 cross compiler if you need to develop i686 Windows program:

```sh
sudo apt-get install g++-mingw-w64-i686
```

Install Mingw-w64 cross compiler if you need to develop x86_64 Windows program:

```sh
sudo apt-get install g++-mingw-w64-x86-64
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

To develop "light_coroutine", generate Makefile and Eclipse project files by this command:

```sh
cmake -G "Eclipse CDT4 - Unix Makefiles" \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_ECLIPSE_VERSION=3.7 ../light_coroutine
```

If you need to develop Windows program by Mingw-w64 cross compiler, set CMake toolchain file like this:

```sh
cmake -D CMAKE_TOOLCHAIN_FILE=../light_coroutine/toolchain/windows_xp-i686.cmake \
    -DCMAKE_BUILD_TYPE=Release ../light_coroutine
```

CMake toolchain files are in the "toolchain" directory.

Relationships between CMake toolchain file and operating system:

| CMake toolchain file             | operating system           |
| -------------------------------- | -------------------------- |
| windows_xp-i686.cmake            | Windows XP i686            |
| windows_xp-x86_64.cmake          | Windows XP x86_64          |
| windows_server_2003-i686.cmake   | Windows Server 2003 i686   |
| windows_server_2003-x86_64.cmake | Windows Server 2003 x86_64 |
| windows-i686.cmake               | later Windows i686         |
| windows-x86_64.cmake             | later Windows x86_64       |

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
