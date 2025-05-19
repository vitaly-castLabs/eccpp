# eccpp = ECC++
Various error correction code (ECC) utils in C++

## Building instructions
Linux/macOS:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
cmake --build build --config Release
```

Windows (Clang is optional, but it does produce faster code):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -T clangcl && \
cmake --build build --config Release
```
