#include <memory>
#include <numeric>
#include "benchmark/benchmark.h"

constexpr size_t cacheline_size = 64; // 64 on x86

template <size_t size>
struct Object
{
    char buffer[size];
};

template <size_t size>
char __attribute__((noinline)) function_call_by_value(Object<size> object) 
{
    return std::accumulate(&object.buffer[0], &object.buffer[size], 0);
}

template <size_t size>
char __attribute__((noinline)) function_call_by_reference(const Object<size>& object)
{
    return std::accumulate(&object.buffer[0], &object.buffer[size], 0);
}

template <size_t size>
void BM_call_by_value(benchmark::State& state) {
    Object<size> object;
    for (auto _ : state) {
        char val = function_call_by_value(object);
        benchmark::DoNotOptimize(val);
    }
}

template <size_t size>
void BM_call_by_reference(benchmark::State& state) {
    Object<size> object;
    for (auto _ : state) {
        char val = function_call_by_reference(object);
        benchmark::DoNotOptimize(val);
    }
}

BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 2);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 2);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 4);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 4);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 5);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 5);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 6);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 6);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 7);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 7);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 8);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 8);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 10);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 10);
BENCHMARK_TEMPLATE(BM_call_by_value,     1 << 12);
BENCHMARK_TEMPLATE(BM_call_by_reference, 1 << 12);

BENCHMARK_MAIN();