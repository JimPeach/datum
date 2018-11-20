// memcpy_vs_loop.cpp
//
// Compare performance of memcpy vs loop

#include <cstring>

#include "benchmark/benchmark.h"

template <int N>
struct test_copy_object : public test_copy_object<N-1> {
    int value = 0;
    void set_value(int val) {
        value = val;
        test_copy_object<N-1>::set_value(val + 1);
    }
};

template <>
struct test_copy_object<0> {
    void set_value(int val) {}
};

template <int N>
static void BM_struct_copy(benchmark::State& state) {
    int range = state.range(0) / 8;
    test_copy_object<N>* source = new test_copy_object<N>[range];
    test_copy_object<N>* dest = new test_copy_object<N>[range];
    for (int i = 0; i < range; i++)
        source[i].set_value(1);

    for (auto _ : state) {
        for (int i = 0; i < range; i++) {
            dest[i] = source[i];
        }
    }

    int64_t bytes_per_iteration = int64_t(range) * sizeof(test_copy_object<N>);
    state.SetBytesProcessed(int64_t(state.iterations()) * bytes_per_iteration);

    delete[] source;
    delete[] dest;
}
BENCHMARK_TEMPLATE(BM_struct_copy, 1)->Range(8, 8<<20);
BENCHMARK_TEMPLATE(BM_struct_copy, 2)->Range(8, 8<<20);
BENCHMARK_TEMPLATE(BM_struct_copy, 4)->Range(8, 8<<20);
BENCHMARK_TEMPLATE(BM_struct_copy, 8)->Range(8, 8<<20);

static void BM_memcpy_int(benchmark::State& state) {
    int* source = new int[state.range(0)];
    int* dest = new int[state.range(0)];
    for (int i = 0; i < state.range(0); i++) {
        source[i] = 0xDEADBEEF;
    }

    for (auto _ : state)
        memcpy(dest, source, state.range(0));

    int64_t bytes_per_iteration = int64_t(state.range(0)) * sizeof(int);
    state.SetBytesProcessed(int64_t(state.iterations()) * bytes_per_iteration);

    delete[] source;
    delete[] dest;
}
  
BENCHMARK(BM_memcpy_int)->Range(8, 8<<20);

BENCHMARK_MAIN();