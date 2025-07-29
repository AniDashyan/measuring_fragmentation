# Measuring Fragmentation

## Overview

This project implements a C++ test harness to simulate memory allocation and deallocation patterns that often occur in long-running applications like servers. It measures the effects of internal and external fragmentation over time by repeatedly allocating and freeing small blocks of memory.

It can optionally use [jemalloc](https://github.com/jemalloc/jemalloc) instead of the standard `malloc` for improved memory behavior analysis.

## Features

* Randomized small-object allocation and deallocation
* Tracks total memory allocated and approximate largest free block
* Prints platform memory usage (on Linux)
* Dumps `jemalloc`   internal stats (if enabled)
* Can build with or without jemalloc

## Build & Run Instructions

```bash
# 1. Clone the Repository
git clone https://github.com/AniDashyan/measuring_fragmentation.git
cd measuring_fragmentation

# 2. Optionally Clone and Build jemalloc
git clone https://github.com/jemalloc/jemalloc.git
cd jemalloc
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ../..
```
Note: On MinGW (Windows), make sure `jemalloc` is built as a static library and that you have its headers and `.lib` or `.a` file available.

``` bash
# 3. Build the Project (with or without jemalloc)
# With jemalloc:
cmake -S . -B build -DUSE_JEMALLOC=ON 
cmake --build build

# Without jemalloc:
cmake -S . -B build -DUSE_JEMALLOC=OFF
cmake --build build

```bash
# 4. Run the Executable
./build/frag_test
```

## Example Output

```
Iteration 0 | Allocations: 0 | Total allocated: 0 bytes | Largest free block: 10485760 bytes | Platform memory used: 6217728 bytes
Iteration 1000 | Allocations: 30 | Total allocated: 4312 bytes | Largest free block: 10485760 bytes | Platform memory used: 6365184 bytes
Iteration 2000 | Allocations: 49 | Total allocated: 5576 bytes | Largest free block: 10485760 bytes | Platform memory used: 6369280 bytes
Iteration 3000 | Allocations: 21 | Total allocated: 2734 bytes | Largest free block: 10485760 bytes | Platform memory used: 6369280 bytes
Iteration 4000 | Allocations: 34 | Total allocated: 4229 bytes | Largest free block: 10485760 bytes | Platform memory used: 6369280 bytes
Iteration 5000 | Allocations: 50 | Total allocated: 6543 bytes | Largest free block: 10485760 bytes | Platform memory used: 6369280 bytes
Iteration 6000 | Allocations: 22 | Total allocated: 2923 bytes | Largest free block: 10485760 bytes | Platform memory used: 6369280 bytes
Iteration 7000 | Allocations: 68 | Total allocated: 8592 bytes | Largest free block: 10485760 bytes | Platform memory used: 6373376 bytes
Iteration 8000 | Allocations: 56 | Total allocated: 7705 bytes | Largest free block: 10485760 bytes | Platform memory used: 6373376 bytes
Iteration 9000 | Allocations: 94 | Total allocated: 13381 bytes | Largest free block: 10485760 bytes | Platform memory used: 6373376 bytes
Iteration 10000 | Allocations: 120 | Total allocated: 17567 bytes | Largest free block: 10485760 bytes | Platform memory used: 6373376 bytes
Iteration 11000 | Allocations: 114 | Total allocated: 15851 bytes | Largest free block: 10485760 bytes | Platform memory used: 6377472 bytes
Iteration 12000 | Allocations: 112 | Total allocated: 14827 bytes | Largest free block: 10485760 bytes | Platform memory used: 6377472 bytes
Iteration 13000 | Allocations: 108 | Total allocated: 14232 bytes | Largest free block: 10485760 bytes | Platform memory used: 6377472 bytes
...
```

## How Does It Work?

* Allocates and frees small memory blocks randomly.
* Periodically:

  * Reports number of active allocations.
  * Tracks total bytes allocated.
  * Approximates the largest block that can still be allocated (external fragmentation).
  * On Linux, reads heap usage via `mallinfo2`.
* When built with jemalloc:

  * Uses `je_malloc` and `je_free` instead of `malloc` and `free`.
  * Calls `je_malloc_stats_print()` to output allocator internals.
