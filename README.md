# eccpp = ECC++
Various error correction code (ECC) utils in C++

## Building and running instructions
Linux/macOS:
```bash
cmake -S . -B build && cmake --build build && ./build/hello-world
```

Windows (Clang is optional, but it does produce faster code), git bash terminal is recommended, unless you enjoy silly slashes:
```bash
cmake -S . -B build -T clangcl && cmake --build build --config Release && ./build/hello-world.exe
```
