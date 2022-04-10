# Parallel programming tasks

## Requirements

* Common:
    * Git (to get source code)
    * CMake 3.0+ for build configuration
* Windows 10:
    * Microsoft Visual C++ Compiler (MSVC) with `Cxx17` support
    * x64 Native Tools Command Prompt for VS
* Ubuntu 20.04:
    * GNU C++ compiler with `Cxx17` support
* MaсOS Catalina 10.15+:
    * Clang C++ compiler with `Cxx17` support

## Instructions

```shell
cmake . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target all -j 4
```

> run from the project root directory

# Executable files

* `build/1_synchorinzation/publisher_subscriber.test.exe`
* `build/2_thread_pool/thread_pool.test.exe`

> `.exe` extension only for Windows platform