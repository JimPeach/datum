// vec_bench.cpp
//
// Compare the performance of dtm::vec and std::vector

#include <vector>
#include "dtm/vec.hpp"

#include "benchmark/benchmark.h"
#include "gperftools/profiler.h"

template <typename V>
static void BM_push_back(benchmark::State& state) {
    size_t num_elements = state.range(0);
    ProfilerStart("vec.prof");
    for (auto _ : state) {
        V vec;
        for (int i = 0; i < num_elements; i++) {
            vec.push_back(i);
        }
    }
    ProfilerStop();
    state.SetItemsProcessed(num_elements * state.iterations());
}

template <typename V>
static void BM_push_back_reserved(benchmark::State& state) {
    size_t num_elements = state.range(0);
    ProfilerStart("vec_reserved.prof");
    for (auto _ : state) {
        V vec;
        vec.reserve(num_elements);
        for (int i = 0; i < num_elements; i++) {
            vec.push_back(i);            
        }
    }
    ProfilerStop();
    state.SetItemsProcessed(num_elements * state.iterations());
}


//BENCHMARK_TEMPLATE(BM_push_back, std::vector<int>)->Range(8,8<<20);
BENCHMARK_TEMPLATE(BM_push_back, dtm::vec<int>)->Range(8,8<<20);
//BENCHMARK_TEMPLATE(BM_push_back_reserved, std::vector<int>)->Range(8,8<<20);
//BENCHMARK_TEMPLATE(BM_push_back_reserved, dtm::vec<int>)->Range(8,8<<20);

BENCHMARK_MAIN();