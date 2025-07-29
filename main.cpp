#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>

#ifdef USE_JEMALLOC
extern "C" {
#include <jemalloc/jemalloc.h>
}
#define MALLOC(size) je_malloc(size)
#define FREE(ptr) je_free(ptr)
#else
#define MALLOC(size) malloc(size)
#define FREE(ptr) free(ptr)
#endif

#ifndef malloc_stats_print
#define malloc_stats_print je_malloc_stats_print
#endif


// Platform-specific memory tracking
#if defined(__linux__)
#include <malloc.h>
size_t get_platform_memory_used() {
    struct mallinfo2 mi = mallinfo2();
    return static_cast<size_t>(mi.uordblks);
}
#elif defined(__APPLE__)
#include <malloc/malloc.h>
size_t get_platform_memory_used() {
    malloc_statistics_t stats;
    malloc_zone_statistics(malloc_default_zone(), &stats);
    return static_cast<size_t>(stats.size_in_use);
}
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
size_t get_platform_memory_used() {
    PROCESS_MEMORY_COUNTERS counters;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters))) {
        return static_cast<size_t>(counters.WorkingSetSize);
    }
    return 0;
}
#else
size_t get_platform_memory_used() {
    return 0;
}
#endif

// Jemalloc stats printing
#ifdef USE_JEMALLOC
void print_jemalloc_stats_stdout() {
    malloc_stats_print(nullptr, nullptr, nullptr);
}
#else
void print_jemalloc_stats_stdout() {
    std::cout << "jemalloc not enabled; no stats available.\n";
}
#endif

// Allocation tracking
struct Allocation {
    void* ptr;
    size_t size;
};

constexpr int MAX_ALLOCATIONS = 10000;
constexpr int MIN_SIZE = 16;
constexpr int MAX_SIZE = 256;
constexpr int ITERATIONS = 100000;
constexpr int REPORT_INTERVAL = 1000;

std::vector<Allocation> allocations;
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<size_t> size_dist(MIN_SIZE, MAX_SIZE);
std::uniform_int_distribution<int> op_dist(0, 1);
size_t total_allocated = 0;

// Binary search approximation of largest free block
size_t get_largest_free_block_approx() {
    size_t low = 1024;
    size_t high = 10 * 1024 * 1024;
    size_t result = 0;
    void* test_ptr = nullptr;

    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        test_ptr = MALLOC(mid);
        if (test_ptr) {
            result = mid;
            FREE(test_ptr);
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

void try_allocate() {
    if (allocations.size() >= MAX_ALLOCATIONS) return;

    size_t size = size_dist(rng);
    void* ptr = MALLOC(size);
    if (ptr) {
        allocations.push_back({ptr, size});
        total_allocated += size;
    }
}

void try_free() {
    if (allocations.empty()) return;

    std::uniform_int_distribution<size_t> index_dist(0, allocations.size() - 1);
    size_t index = index_dist(rng);

    total_allocated -= allocations[index].size;
    FREE(allocations[index].ptr);

    allocations[index] = allocations.back();
    allocations.pop_back();
}

int main() {
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < ITERATIONS; ++i) {
        if (op_dist(rng) == 0) {
            try_allocate();
        } else {
            try_free();
        }

        if (i % REPORT_INTERVAL == 0) {
            size_t largest_block = get_largest_free_block_approx();
            size_t mem_used = get_platform_memory_used();

            std::cout << "Iteration " << i
                      << " | Allocations: " << allocations.size()
                      << " | Total allocated: " << total_allocated << " bytes"
                      << " | Largest free block: " << largest_block << " bytes"
                      << " | Platform memory used: " << mem_used << " bytes\n";
        }
    }

    for (auto& a : allocations) {
        FREE(a.ptr);
    }
    allocations.clear();

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total run time: " << elapsed.count() << " ms\n";

    std::cout << "\n=== Final jemalloc stats ===\n";
    print_jemalloc_stats_stdout();

    return 0;
}
