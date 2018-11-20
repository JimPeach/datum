// realloc_vs_copy.cpp
//
// Compare performance of realloc vs memcpy

#include <cstring>
#include <cstdlib>
#include <array>

#include "benchmark/benchmark.h"

struct resize_with_memcpy
{
    char* operator() (char* old_buffer, size_t old_size, size_t new_size)
    {
        char* new_buffer = (char*) malloc(new_size);
        memcpy(new_buffer, old_buffer, std::min(old_size, new_size));
        free(old_buffer);
        return new_buffer;
    }
};

struct resize_with_realloc
{
    char* operator() (char* old_buffer, size_t old_size, size_t new_size)
    {
        return (char*) realloc(old_buffer, new_size);
    }
};

template <typename T>
static void BM_grow(benchmark::State& state) {
    int range = state.range(0);

    std::array<char*, 128> mems;

    T grow_func;

    for (auto _ : state) {
        //state.PauseTiming();
        for (char*& ptr : mems) {
            ptr = (char*) malloc(range);
            memset(ptr, 'A', range);
        }
        //state.ResumeTiming();

        for (char*& ptr : mems) {
            ptr = grow_func(ptr, range, range * 1.5);
        }
        
        //state.PauseTiming();
        for (auto& ptr : mems) {
            free(ptr);
        }
        //state.ResumeTiming();
    }

    int64_t bytes_per_iteration = int64_t(state.range(0)) * 128;
    state.SetBytesProcessed(int64_t(state.iterations()) * bytes_per_iteration);
}
  
BENCHMARK_TEMPLATE(BM_grow, resize_with_memcpy)->Range(8, 8<<20);
BENCHMARK_TEMPLATE(BM_grow, resize_with_realloc)->Range(8, 8<<20);

BENCHMARK_MAIN();