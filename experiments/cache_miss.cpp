// memcpy_vs_loop.cpp
//
// Compare performance of memcpy vs loop

#include <memory>
#include <cassert>
#include <numeric>
#include "benchmark/benchmark.h"

constexpr size_t cacheline_size = 64; // 64 on x86

void BM_cached_row_sum(benchmark::State& state) {
    constexpr size_t ints_in_cacheline = cacheline_size / sizeof(int); 
    int buffer[2 * ints_in_cacheline];
    void* aligned_buffer_vp = static_cast<void*>(buffer);
    size_t sizeof_aligned_buffer = sizeof(buffer);
    std::align(cacheline_size, cacheline_size, aligned_buffer_vp, sizeof_aligned_buffer);

    int* aligned_buffer = static_cast<int*>(aligned_buffer_vp);

    for (auto _ : state) {
        int accum = std::accumulate(aligned_buffer, aligned_buffer + ints_in_cacheline, 0);
        benchmark::DoNotOptimize(accum);
    }
}

void BM_uncached_row_sum(benchmark::State& state) {
    constexpr size_t ints_in_cacheline = cacheline_size / sizeof(int); 
    int buffer[2 * ints_in_cacheline];
    void* aligned_buffer_vp = static_cast<void*>(buffer);
    size_t sizeof_aligned_buffer = sizeof(buffer);
    std::align(cacheline_size, cacheline_size, aligned_buffer_vp, sizeof_aligned_buffer);

    int* aligned_buffer = static_cast<int*>(aligned_buffer_vp);

    for (auto _ : state) {
        __builtin_ia32_clflush(aligned_buffer);
        int accum = std::accumulate(aligned_buffer, aligned_buffer + ints_in_cacheline, 0);
        benchmark::DoNotOptimize(accum);
    }
}

BENCHMARK(BM_cached_row_sum);
BENCHMARK(BM_uncached_row_sum);

BENCHMARK_MAIN();