# Windows C++ with Bazel

Bazel's C++ toolchain setup on Windows is genuinely complex and can seem contradictory.

## Windows C++ Compilation Complexity

The complexity exists because Windows C++ development historically fragmented across different vendors and approaches,
and Bazel tries to support multiple scenarios while maintaining compatibility with its Unix origins.

Windows doesn't have a standardized system compiler like Linux (gcc) or macOS (clang). Instead, there are multiple
competing toolchains, each with their own quirks:

- **MSVC** (Microsoft Visual C++) - Microsoft's native compiler and toolchain
- **Clang** - LLVM's compiler that can target Windows
- **MinGW/GCC** - GNU toolchain ported to Windows

It requires all three components:

### MSVC (Visual Studio/Build Tools)

Provides the Windows SDK and essential headers/libraries. Contains cl.exe (the MSVC compiler) and the Microsoft linker
Even when using Clang, often needs MSVC's standard library and Windows headers. The "Community" version is free and
mostly sufficient.

### MSYS2

Provides Unix-like tools that Bazel expects (bash, find, grep, etc.). Bazel was originally designed for Unix-like
systems and relies on these tools
Without MSYS2, many Bazel rules and scripts will fail. Acts as a compatibility layer between Bazel's Unix assumptions
and Windows.

### Clang/LLVM

Often preferred over MSVC's cl.exe for better C++ standards compliance. clang-cl.exe is a special version that accepts
MSVC-style command line arguments
Can use MSVC's standard library and linker while providing better compilation.

### The cl vs clang-cl Confusion

Bazel switches between these, it's because:

**cl.exe** - Microsoft's compiler with Microsoft-specific syntax   
**clang-cl.exe** - Clang compiler that mimics cl.exe's command-line interface   
**clang.exe** - Standard Clang with GCC-style arguments

Bazel might configure different toolchains depending on setup. clang-cl is often preferred because it combines Clang's
better error messages and standards compliance with compatibility for existing MSVC-based build systems.

## Setup

### Install Visual Studio Build Tools (for Windows SDK and MSVC libraries)

### Install MSYS2 (for Unix tools)

`choco install msys2`

```
[System.Environment]::SetEnvironmentVariable("BAZEL_SH", "C:\tools\msys64\usr\bin\bash.exe", "User")
```

Add `C:\msys64\usr\bin` to path.

### Install Clang/LLVM

`choco install llvm`

