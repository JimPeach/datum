cmake_minimum_required (VERSION 3.8)

set(CMAKE_BUILD_TYPE Release)

add_executable (datum_vec_bench "vec_bench.cpp")
target_compile_options (datum_vec_bench PUBLIC "-std=c++14")
target_link_libraries (datum_vec_bench benchmark pthread)
