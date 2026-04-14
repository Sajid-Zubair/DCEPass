# DCEPass — Dead Code Elimination Pass for LLVM

A custom LLVM function pass that eliminates dead instructions from LLVM IR. Built as a loadable plugin using the LLVM New Pass Manager, it integrates seamlessly into the `opt` optimization pipeline.

---

## How It Works

The pass iterates over every basic block in a function and removes instructions that satisfy all three conditions:

- **No uses** — the instruction's result is never referenced (`use_empty()`)
- **Not a terminator** — branch/return instructions are preserved
- **No side effects** — instructions like function calls with observable effects are left alone

This handles common cases of dead code such as unused arithmetic, redundant loads, and intermediate values that get optimized away.

---

## Prerequisites

- **LLVM** (version 14+ recommended) with development headers
- **CMake** 3.13+
- **Clang** (to compile test files to IR)
- A Unix-like system (Linux / macOS)

On Ubuntu/Debian, install LLVM with:

```bash
sudo apt install llvm llvm-dev clang cmake
```

---

## Building

```bash
git clone https://github.com/Sajid-Zubair/DCEPass.git
cd DCEPass
mkdir build && cd build
cmake ..
make
```

This produces a shared library: `build/DCEPass.so` (or `DCEPass.dylib` on macOS).

---

## Usage

### 1. Compile your test file to LLVM IR

```bash
clang -O0 -S -emit-llvm test.cpp -o test.ll
```

The `-O0` flag ensures no optimizations are applied beforehand, so the dead code is still present in the IR.

### 2. Run the pass with `opt`

```bash
opt -load-pass-plugin ./build/DCEPass.so -passes="dce-pass" -S test.ll -o out.ll
```

### 3. Inspect the output

```bash
cat out.ll
```

Compare `test.ll` and `out.ll` — dead instructions that had no uses and no side effects will be gone.

---

## Example

Given `test.cpp`:

```cpp
int foo(int x) {
    int unused = x * 42;   // dead — result never used
    return x + 1;
}
```

After compiling to IR and running DCEPass, the `unused = x * 42` multiplication instruction will be removed from the output IR.

---

## Project Structure

```
DCEPass/
├── DCEPass.cpp      # The LLVM pass implementation
├── CMakeLists.txt   # Build configuration
├── test.cpp         # Sample C++ file to test the pass on
├── test.ll          # Pre-generated LLVM IR of test.cpp
└── out.ll           # Output IR after running the pass
```

---

## Notes

- The pass uses LLVM's **New Pass Manager** API (`PassInfoMixin`, `PassBuilder`).
- The plugin name registered with `opt` is `dce-pass`.
- The commented-out code in `DCEPass.cpp` is an alternative version that targets dead stores and unused `alloca` instructions specifically — it can be swapped in to experiment with a different DCE strategy.

---

## License

This project is open source. Feel free to use, modify, and learn from it.